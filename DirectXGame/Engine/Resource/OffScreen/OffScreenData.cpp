#include "OffScreenData.h"
#include <Core/DXCommonFunction.h>

OffScreenData::OffScreenData(int width, int height, DXDevice* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srv, ID3D12DescriptorHeap* rtv) {

    //PostEffect用のリソースの作成
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;              // defaultのヒープを使用

    HRESULT hr = device->GetDevice()->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&textureResource_)
    );
    assert(SUCCEEDED(hr) && "Failed to create off-screen resource");

    rtvHandle_.ptr = rtv->GetCPUDescriptorHandleForHeapStart().ptr + device->GetDescriptorSizeRTV() * (2 + offScreenTextureCount_++);

    //RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDescOffScreen{};
    rtvDescOffScreen.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//出力結果をSRGBに変換して書き込む
    rtvDescOffScreen.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとしてよみこむ
    device->GetDevice()->CreateRenderTargetView(textureResource_.Get(), &rtvDescOffScreen, rtvHandle_);

    // metadataがないのでフォーマットとミップ数は手動設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    //!mipmapを使うかどうかは今後要検討
    srvDesc.Texture2D.MipLevels = 1;

    // SRV用ディスクリプタ位置を確保
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDesscriptorHandle(
        srv, device->GetDescriptorSizeSRV(), offScreenTextureCount_);
    gpuHandle_ = GetGPUDesscriptorHandle(
        srv, device->GetDescriptorSizeSRV(), offScreenTextureCount_);

    // SRVを作成
    device->GetDevice()->CreateShaderResourceView(textureResource_.Get(), &srvDesc, textureSrvHandleCPU);

    //バリアの初期状態を設定
    resourceState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

}
