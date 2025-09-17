#include "Render.h"
#include <Core/DXCommonFunction.h>

Render::Render(DXDevice* device) {
    device_ = device;
    logger_ = std::make_unique<Logger>();
	logger_->RegistLogFile("Render");
    psoEditor_ = std::make_unique<PSOEditor>(device_->GetDevice());
}

Render::~Render() {
}

void Render::Initialize() {

#pragma region CommandQueue

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    HRESULT hr = device_->GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    //コマンドキューの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create CommandQueue\n");

#pragma endregion

#pragma region CommandAllocator

    hr = device_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    //コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create CommandAllocator\n");

#pragma endregion

#pragma region CommandList

    hr = device_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    //コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create CommandList\n");

#pragma endregion

	//テクスチャマネージャーの生成
    textureManager_ = std::make_unique<TextureManager>(device_, commandList);

	auto windowSize = device_->GetWindowSize();

    //スワップチェーンを生成する
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = windowSize.first;	                        //ウィンドウ幅
    swapChainDesc.Height = windowSize.second;	                    //ウィンドウ高さ
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      	    //色の形式
    swapChainDesc.SampleDesc.Count = 1;	                            //マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2;	//ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   	//モニタに映したら捨てる

    //コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr = device_->GetDxgiFactory()->CreateSwapChainForHwnd(
        commandQueue.Get(),		        		//コマンドキュー
        device_->GetHwnd(),			            //ウィンドウハンドル
        &swapChainDesc,	        		        //設定
        nullptr,		    	    		    //モニタの設定
        nullptr,			    		        //出力の設定
        reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));	//スワップチェーンのポインタ
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create SwapChain\n");

    //SwapChainからResourceを取得する
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    //ディスクリプタヒープの生成(swapchain用なので二つ)
    rtvDescriptorHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    logger_->Log("Complete create DescriptorHeap\n");

    //RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//出力結果をSRGBに変換して書き込む
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとしてよみこむ
    //ディスクリプタの先頭を取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //まず一つ目をつくる。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある。
    rtvHandles[0] = rtvStartHandle;
    device_->GetDevice()->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
    //二つ目のディスクリプタハンドルを得る
    rtvHandles[1].ptr = rtvHandles[0].ptr + device_->GetDescriptorSizeRTV();
    //二つ目を作る
    device_->GetDevice()->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
    logger_->Log("Complete create RenderTargetView\n");

#pragma endregion

#pragma region Fence

    //初期値0でフェンスを作る
    hr = device_->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    //FenceのSignalを待つためのイベントを作成する
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);

#pragma endregion

    //=======================================================

    dsvDescriptorHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    depthStencilResource = CreateDepthStencilTextureResource(device_->GetDevice(), windowSize.first, windowSize.second);

    device_->GetDevice()->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    //PSOの初期化
    psoEditor_ = std::make_unique<PSOEditor>(device_->GetDevice());
    psoEditor_->Initialize(device_->GetDevice());
}

void Render::Draw(DXResource* resource) {

    commandList->IASetVertexBuffers(0, 1, &resource->GetVertexBufferView());
	uint32_t indexNum = resource->GetIndexNum();
    if (indexNum != 0) {
		commandList->IASetIndexBuffer(&resource->GetIndexBufferView());
    }

    //マテリアルのポインタを設定
    commandList->SetGraphicsRootConstantBufferView(0, resource->GetMaterialResource()->GetGPUVirtualAddress());
    //Matrixのポインタを設定
    commandList->SetGraphicsRootConstantBufferView(1, resource->GetMatrixResource()->GetGPUVirtualAddress());
    //Texture
    commandList->SetGraphicsRootDescriptorTable(2, textureManager_->GetTextureData(resource->textureHandle_)->GetTextureGPUHandle());
	//Lightのポインタを設定
    commandList->SetGraphicsRootConstantBufferView(3, resource->GetLightResource()->GetGPUVirtualAddress());

    psoEditor_->SetPSOConfig(resource->psoConfig_);
    psoEditor_->Setting(commandList.Get());

    if (indexNum != 0) {
        //インデックスがある場合は、インデックスを設定して描画
        commandList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
    } else {
        //インデックスがない場合は、インデックスなしで描画
        commandList->DrawInstanced(resource->GetVertexNum(), 1, 0, 0);
    }

}
