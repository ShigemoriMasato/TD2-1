#include "MyDirectX.h"
#include <cstdint>
#include <cassert>
#include <strsafe.h>
#include "../Math/MyMath.h"
#include <memory>
#include <sstream>
#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_dx12.h"
#include "../../externals/imgui/imgui_impl_win32.h"
#include "../../externals/DirectXTex/DirectXTex.h"
#include "../../externals/DirectXTex/d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

using namespace MyMath;

namespace {
    //ウィンドウプロシージャ
    LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        //imguiのウィンドウプロシージャを呼ぶ
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
            return true;
        }

        //メッセージに応じてゲーム固有の処理を行う
        switch (msg) {
        case WM_DESTROY:
            //アプリを落とす
            PostQuitMessage(0);
            return 0;
        }

        //標準のメッセージ
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
        //取得した時間を名前にしたファイルを作成し、そのなかにDumpを出力する
        SYSTEMTIME time;
        GetLocalTime(&time);
        wchar_t filePath[MAX_PATH] = { 0 };
        CreateDirectory(L"./Dumps", nullptr);
        StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
        HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
        //processID(このexeのID)とクラッシュ(例外)の発生したthreadIDを取得
        DWORD processId = GetCurrentProcessId();
        DWORD threadId = GetCurrentThreadId();
        //設定情報を入力
        MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
        minidumpInformation.ThreadId = threadId;
        minidumpInformation.ExceptionPointers = exception;
        minidumpInformation.ClientPointers = TRUE;
        //Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
        MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
        //他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
        return EXCEPTION_EXECUTE_HANDLER;
    }

    IDxcBlob* CompileShader(
        //CompilerするShaderファイルへのパス
        const std::wstring& filePath,
        //Compilerに使用するprofile
        const wchar_t* profile,
        //初期化で生成したものを3つ
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler,
        Logger* logger) {

        logger->Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", filePath, profile)));

        //hlslファイルを読む
        IDxcBlobEncoding* shaderSource = nullptr;
        HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
        //読めなかったら止める
        assert(SUCCEEDED(hr));
        //読み込んだファイルの内容を設定する
        DxcBuffer shaderSourceBuffer;
        shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
        shaderSourceBuffer.Size = shaderSource->GetBufferSize();
        shaderSourceBuffer.Encoding = DXC_CP_UTF8;//utf8の文字コードであることを通知

        LPCWSTR arguments[] = {
            filePath.c_str(),	//コンパイル対象のhlslファイル名
            L"-E", L"main",     //エントリーポイントの指定。基本的にmain以外には市内
            L"-T", profile,    //ShaderProfileの設定
            L"-Zi", L"-Qembed_debug", //デバッグ用の情報を埋め込む
            L"-Od",     //最適化を行わない
            L"-Zpr",     //メモリレイアウトは行優先
        };
        //実際にShaderをコンパイルする
        IDxcResult* shaderResult = nullptr;
        hr = dxcCompiler->Compile(
            &shaderSourceBuffer,	//読み込んだファイル
            arguments,			    //コンパイルオプション
            _countof(arguments),	//コンパイルオプションの数
            includeHandler,		    //includeが含まれた諸々
            IID_PPV_ARGS(&shaderResult)		//コンパイル結果
        );
        //コンパイルエラーではなくdxcが起動できないなどの致命的な状況
        assert(SUCCEEDED(hr));

        //警告・エラーが出てたらログに出して止める
        IDxcBlobUtf8* shaderError = nullptr;
        shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
        if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
            logger->Log(shaderError->GetStringPointer());
            //警告・エラーが起きている状態なので止める
            assert(false);
        }

        //コンパイル結果から実行用のバイナリ部分を取得
        IDxcBlob* shaderBlob = nullptr;
        hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
        assert(SUCCEEDED(hr));
        //成功したログを出す
        logger->Log(ConvertString(std::format(L"Compile Successd, path: {}, profile: {}\n", filePath, profile)));
        //もう使わないリソースを開放
        shaderSource->Release();
        shaderResult->Release();
        //実行用のバイナリを返却
        return shaderBlob;
    }

    ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
        //頂点リソース用のヒープの設定
        D3D12_HEAP_PROPERTIES uploadHeapProperties{};
        uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//uploadHeapを使う
        //頂点リソースの設定
        D3D12_RESOURCE_DESC bufferResourceDesc{};
        //バッファリソース、テクスチャの場合はまた別の設定をする
        bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferResourceDesc.Width = sizeInBytes;
        //バッファの場合はこれにする決まり
        bufferResourceDesc.Height = 1;
        bufferResourceDesc.DepthOrArraySize = 1;
        bufferResourceDesc.MipLevels = 1;
        bufferResourceDesc.SampleDesc.Count = 1;
        //バッファの場合はこれにする決まり
        bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        ID3D12Resource* bufferResource = nullptr;

        HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
            &bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(&bufferResource));
        assert(SUCCEEDED(hr));

        return bufferResource;
    }

    ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible) {
        //ディスクリプタヒープの設定
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
        descriptorHeapDesc.Type = type;	//ディスクリプタの種類
        descriptorHeapDesc.NumDescriptors = numDescriptors;	//ディスクリプタの数
        descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	//シェーダーからアクセスできるようにする
        ID3D12DescriptorHeap* descriptorHeap = nullptr;
        HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
        assert(SUCCEEDED(hr));
        return descriptorHeap;
    }

    DirectX::ScratchImage CreateMipImages(const std::string& filePath) {
        //テクスチャファイルを読んでプログラムで扱えるようにする
        DirectX::ScratchImage image{};
        std::wstring filePathW = ConvertString(filePath);
        HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
        assert(SUCCEEDED(hr));

        DirectX::ScratchImage mipImages{};
        //ミニマップの作成(画像サイズが最小の場合、作成手順を飛ばす)
        if (image.GetMetadata().mipLevels > 1) {
            hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
            assert(SUCCEEDED(hr));
        }
        else {
			mipImages = std::move(image);
        }
        return mipImages;
    }

    ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
        //metadataを基にResourceの設定
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = UINT(metadata.width);
        resourceDesc.Height = UINT(metadata.height);
        resourceDesc.MipLevels = UINT16(metadata.mipLevels);
        resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
        resourceDesc.Format = metadata.format;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

        //利用するHeapの設定。非常に特殊な運用。02-04exで一般的なケース版がある。
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        //Resourceの生成
        ID3D12Resource* resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE,
            &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
            IID_PPV_ARGS(&resource));
        assert(SUCCEEDED(hr));
        return resource;
    }

    ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
        //生成するResourceの設定
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = width;
        resourceDesc.Height = height;
        resourceDesc.MipLevels = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        //利用するヒープの設定
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        //深度値のクリア設定
        D3D12_CLEAR_VALUE depthClearValue{};
        depthClearValue.DepthStencil.Depth = 1.0f;
        depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        //Resourceの生成
        ID3D12Resource* resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthClearValue,
            IID_PPV_ARGS(&resource));
        assert(SUCCEEDED(hr));

        return resource;
    }

    [[nodiscard]]
    ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
        uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
        ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
        UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = texture;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &barrier);
        return intermediateResource;
    }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDesscriptorHandle(ID3D12DescriptorHeap* heap,uint32_t descriptorSize , UINT index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (descriptorSize * index);
		return handle;
	}

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDesscriptorHandle(ID3D12DescriptorHeap* heap, uint32_t descriptorSize, UINT index) {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
        handle.ptr += (descriptorSize * index);
        return handle;
    }
}

[[nodiscard]]
int MyDirectX::LoadTexture(std::string path) {
    //TextureResourceを作成
    DirectX::ScratchImage mipImages = CreateMipImages(path);
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    textureResource.push_back(CreateTextureResource(device.Get(), metadata));
    ID3D12Resource* resource = UploadTextureData(textureResource.back(), mipImages, device.Get(), commandList.Get());
	intermediateResource.push_back(resource);

    //metadataをもとにSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    //SRVを作成するDescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    textureSrvHandleGPU.push_back(srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    //先頭はImGuiが使っているのでその次を使う
    textureSrvHandleCPU = GetCPUDesscriptorHandle(srvDescriptorHeap, descriptorSizeSRV, ++readTextureCount);
    textureSrvHandleGPU.back() = GetGPUDesscriptorHandle(srvDescriptorHeap, descriptorSizeSRV, readTextureCount);

    //SRVを作成する
    device->CreateShaderResourceView(textureResource.back(), &srvDesc, textureSrvHandleCPU);

    return readTextureCount - 1;
}

[[nodiscard]]
int MyDirectX::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
    ModelData modelData;                    //構築するデータ
    std::vector<Vector4> positions;         //位置
	std::vector<Vector3> normals;           //法線
    std::vector<Vector2> texcoords;         //テクスチャ座標
    std::string line;                       //ファイルから読んだ行を格納するバッファ

	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
    assert(file.is_open() && "MyDirectX::LoadObjFile / cannot open obj file");

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier; //行の先頭の文字列を取得

        if (identifier == "v") {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.w = 1.0f;
            positions.push_back(position); //位置を格納
        } else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y; //テクスチャ座標を格納
			texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        } else if (identifier == "vn") {
            Vector3 normal;
			s >> normal.x >> normal.y >> normal.z; //法線を格納
            normal.x *= -1.0f; //y軸も反転
			normals.push_back(normal);
        } else if (identifier == "f") {
            //面は三角形限定なので、読み込む前にEditor等で三角化させること。そのほかは未対応
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
				s >> vertexDefinition; //頂点の定義を取得
                //頂点の要素へのIndexは。「位置/UV/法線」で格納されているので、分解してIndexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');// /区切りでインデックスを読む
                    elementIndices[element] = std::stoi(index);
                }
                //要素へのIndexから、実際の要素の値を取得して頂点を構築する
                Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
                VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex); //頂点を格納
            }
        } else if (identifier == "mtllib") {
            std::string materialFilename;
            s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename); //マテリアルファイルを読み込む
        }
    }
    
	modelList_.push_back(modelData); //モデルデータをリストに追加
	drawCount.push_back(0); //描画数を初期化
	
    vertexResource.push_back(std::vector<ID3D12Resource*>()); //頂点リソースの初期化
	indexResource.push_back(std::vector<ID3D12Resource*>()); //インデックスリソースの初期化
	materialResource.push_back(std::vector<ID3D12Resource*>()); //マテリアルリソースの初期化
	wvpResource.push_back(std::vector<ID3D12Resource*>()); //ワールドビュー投影行列リソースの初期化
	directionalLightResource.push_back(std::vector<ID3D12Resource*>()); //DirectionalLightリソースの初期化

    modelCount_++;

    return int(modelList_.size() - 1);
}

MyDirectX::MyDirectX(int32_t kWindowWidth, int32_t kWindowHeight) :
    kClientWidth(kWindowWidth),
    kClientHeight(kWindowHeight),
    clearColor(new float[4] {0.1f, 0.25f, 0.5f, 1.0f}),
    logger(new Logger("master")),
    fenceValue(0),
    readTextureCount(0),
    modelCount_(-1),
    isCanDraw_(new bool(false)) {
    resourceStates[swapChainResources[0].Get()] = D3D12_RESOURCE_STATE_PRESENT;
    resourceStates[swapChainResources[1].Get()] = D3D12_RESOURCE_STATE_PRESENT;
	myWindow_ = new MyWindow(kWindowWidth, kWindowHeight);
	pso = new MyPSO();
    Initialize();
}

MyDirectX::~MyDirectX() {
    Finalize();
    delete myWindow_;
	delete isCanDraw_;
    delete logger;
    delete[] clearColor;
	delete pso;
}

void MyDirectX::Initialize() {
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));
    frame_ = 0;

    wndHandle_ = myWindow_->CreateWindowForApp();
    InitDirectX();
    InitImGui();
    //0と1の固定番号として扱うためここだけ返り値を破棄
    int a = LoadTexture("resources/uvChecker.png");
    a = LoadTexture("resources/white1x1.png");
}

int MyDirectX::CreateDrawResource(DrawKind drawKind, uint32_t createNum) {
    for (uint32_t i = 0; i < createNum; ++i) {
		//頂点リソース用のリソース作成
        switch (drawKind) {
        case kSphere:
			vertexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(VertexData) * 992 * 3));
            break;

		case kSprite:
			vertexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(VertexData) * 4));
			indexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(uint32_t) * 6));
			break;

        case kPrism:
			vertexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(VertexData) * 7));
			indexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(uint32_t) * 24));
            break;

        case kBox:
			vertexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(VertexData) * 24));
			indexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(uint32_t) * 36));
            break;

        default:
            vertexResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(VertexData) * 3));
            break;
        }
		//wvpMatrixのリソース作成
		wvpResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(TramsformMatrixData)));
		//マテリアル用のリソースリソース作成
		materialResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(MaterialData)));
        //DirectionalLight用のリソース作成
		directionalLightResource[drawKind].push_back(CreateBufferResource(device.Get(), sizeof(DirectionalLightData)));
    }

	return int(vertexResource[drawKind].size()); //描画できる最大数を返す
}

int MyDirectX::CreateModelDrawResource(uint32_t modelHandle, uint32_t createNum) {
    uint32_t index = DrawKindCount + modelHandle;

    for (uint32_t i = 0; i < createNum; ++i) {
		vertexResource[index].push_back(CreateBufferResource(device.Get(), sizeof(VertexData) * modelList_[modelHandle].vertices.size()));
		materialResource[index].push_back(CreateBufferResource(device.Get(), sizeof(MaterialData)));
		directionalLightResource[index].push_back(CreateBufferResource(device.Get(), sizeof(DirectionalLightData)));
		wvpResource[index].push_back(CreateBufferResource(device.Get(), sizeof(TramsformMatrixData)));
    }

	return int(vertexResource[index].size()); //描画できる最大数を返す
}

ModelMaterial MyDirectX::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
    ModelMaterial material = {};
    std::string textureFilePath;
    std::string line;
	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
    assert(file.is_open() && "MyDirectX::LoadMaterialTemplateFile cannot open the mtlFile");

    while (std::getline(file, line)) {
        std::string identifier;
        std::stringstream s(line);
		s >> identifier;

        //identifierに応じて処理
        if (identifier == "map_Kd") {
            std::string textureFilename;
			s >> textureFilename;
            //連結してファイルパスにする
			textureFilePath = directoryPath + "/" + textureFilename;
            material.textureHandle = LoadTexture(textureFilePath);
        }
    }

    if (material.textureHandle == 0) {
		//テクスチャが読み込めなかった場合は白い1x1のテクスチャを使う
		material.textureHandle = 1;
    }

    return material;
}

void MyDirectX::BeginFrame() {
    BeginImGui();
}

void MyDirectX::PreDraw() {
    ClearScreen();
    *isCanDraw_ = true;
}

void MyDirectX::InitDirectX() {
#ifdef _DEBUG

    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        //デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        //さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

#pragma region dxgiFactory
    //関数が成功したかどうかをSUCCEEDEDマクロで判定する
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region Adapter
    //一番いいアダプタを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
        //アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr = useAdapter->GetDesc3(&adapterDesc);
        //取得できなければエラー
        assert(SUCCEEDED(hr));
        //ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            //採用したアダプタの情報をログに出力
            logger->Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));

            break;
        }
        //ソフトウェアアダプタの場合はなかったことに
        useAdapter = nullptr;
    }

    //適切なアダプタが見つからなかった場合は起動できない
    assert(useAdapter != nullptr);
#pragma endregion

#pragma region Device
    //機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };
    std::string featureLevelStrings[] = { "12.2","12.1","12.0" };
    //高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        //採用したアダプタでデバイスを生成
        hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
        //指定した機能レベルでデバイスが生成できたか確認
        if (SUCCEEDED(hr)) {
            //生成できたのでログを出力してループを抜ける
            logger->Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
            break;
        }
    }

    //デバイスの生成が上手くいかなかったので起動できない
    assert(device != nullptr);
    logger->Log("Complete create D3D12Device\n");

#pragma endregion

#pragma region DescriptorSizeの取得

	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

#pragma endregion

#pragma region DebugLayer

#ifdef _DEBUG

    ID3D12InfoQueue* infoQueue = nullptr;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        //やばいエラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        //エラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        //警告の時に止まる(これをコメントアウトすると解放していないObjectを特定できる(?))
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        //抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            //windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            //https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        //抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        //指定したメッセージの行事を抑制する
        infoQueue->PushStorageFilter(&filter);

        //解放
        infoQueue->Release();
    }

#endif

#pragma endregion

    //=======================================================
    //          画面を塗りつぶす素材用意
    //=======================================================

#pragma region CommandQueue

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    //コマンドキューの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger->Log("Complete create CommandQueue\n");

#pragma endregion

#pragma region CommandAllocator

    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    //コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger->Log("Complete create CommandAllocator\n");

#pragma endregion

#pragma region CommandList

    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    //コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger->Log("Complete create CommandList\n");

#pragma endregion

#pragma region SwapChain

    //スワップチェーンを生成する
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = kClientWidth;	                            //ウィンドウ幅
    swapChainDesc.Height = kClientHeight;	                        //ウィンドウ高さ
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      	    //色の形式
    swapChainDesc.SampleDesc.Count = 1;	                            //マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2;	//ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   	//モニタに映したら捨てる

    //コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),		        		//コマンドキュー
        myWindow_->GetHwnd(wndHandle_),			           			//ウィンドウハンドル
        &swapChainDesc,	        		    //設定
        nullptr,		    	    		//モニタの設定
        nullptr,			    		    //出力の設定
        reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));	//スワップチェーンのポインタ
    assert(SUCCEEDED(hr));
    logger->Log("Complete create SwapChain\n");

#pragma endregion

#pragma region SwapChainResource

    //SwapChainからResourceを取得する
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

#pragma endregion

#pragma region DescriptorHeap

    //ディスクリプタヒープの生成
    rtvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    //ディスクリプタヒープの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger->Log("Complete create DescriptorHeap\n");

#pragma endregion

#pragma region RenderTargetView

    //RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//出力結果をSRGBに変換して書き込む
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとしてよみこむ
    //ディスクリプタの先頭を取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //まず一つ目をつくる。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある。
    rtvHandles[0] = rtvStartHandle;
    device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
    //二つ目のディスクリプタハンドルを得る(自力で)
    rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    //二つ目を作る
    device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
    logger->Log("Complete create RenderTargetView\n");

#pragma endregion

#pragma region Fence

    //初期値0でフェンスを作る
    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    //FenceのSignalを待つためのイベントを作成する
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);

#pragma endregion

    //=======================================================

#pragma region dxcUtils & dxcCompiler

    //dxcCompilerを初期化
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

#pragma endregion

#pragma region includeHandler

    //現時点でincludeはしないが、includeに対応するための設定を行っておく
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));

#pragma endregion

#pragma region RootSignature

    //RootSignature作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region RootParameter

#pragma region DescriptorTable

    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

    //RootParameter作成。複数設定できるので配列
    D3D12_ROOT_PARAMETER rootParameters[4] = {};

#pragma region Material
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;     //PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド
#pragma endregion

#pragma region Matrix
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;     //VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド
#pragma endregion

#pragma region Texture

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//テーブルを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;	//テーブルの中身
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//テーブルの数

#pragma endregion

#pragma region DirectionalLight
    
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;	//レジスタ番号1とバインド

#pragma endregion

#pragma endregion

    descriptionRootSignature.pParameters = rootParameters;                  //ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters);      //配列の長さ

#pragma endregion



    D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
    staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//フィルタリングの方法
    staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//比較しない
    staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;    //ありったけのMipmapを使う
    staticSampler[0].ShaderRegister = 0;    //レジスタ番号0
    staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShaderで使う
    descriptionRootSignature.pStaticSamplers = staticSampler;              //StaticSamplerの配列へのポインタ
    descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);   //配列の長さ

    //シリアライズしてバイナリにする
    hr = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        logger->Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }
    //バイナリをもとに生成
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));

    //InputLayout
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    //BlendStateの設定
    D3D12_BLEND_DESC blendDesc{};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    //すべての色を取り込む
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    //RasisterzerStateの設定
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    //裏面(時計回り)を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    //三角形の中を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    //Shaderをコンパイルする
    vertexShaderBlob = CompileShader(L"./Engine/Shader/Object3D.VS.hlsl",
        L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get(), logger);
    assert(vertexShaderBlob != nullptr);

    pixelShaderBlob = CompileShader(L"./Engine/Shader/Object3D.PS.hlsl",
        L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get(), logger);
    assert(pixelShaderBlob != nullptr);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

    dsvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    depthStencilResource = CreateDepthStencilTextureResource(device.Get(), kClientWidth, kClientHeight);

    device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;	//深度バッファを使う
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//全ての深度値を使う
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	//深度値の比較方法

    depthStencilDesc.StencilEnable = FALSE; // ステンシルテストを使わないなら FALSE
    depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK; // デフォルト値
    depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK; // デフォルト値

    //実際に生成
    pso->SetPSODesc(depthStencilDesc,
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        rootSignature.Get(),
        inputLayoutDesc,
        { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() },
        { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() },
        blendDesc,
        rasterizerDesc,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        D3D12_DEFAULT_SAMPLE_MASK
        );
	pso->SetDevice(device.Get());
    pso->CreatePSO(int(PSOType::kOpaqueTriangle));

	pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	pso->CreatePSO(int(PSOType::kOpaqueLine));

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc2{};
    depthStencilDesc2.DepthEnable = true;	//深度バッファを使う
    depthStencilDesc2.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;	//深度地を書き込まない
    depthStencilDesc2.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	//深度値の比較方法

    depthStencilDesc2.StencilEnable = FALSE; // ステンシルテストを使わないなら FALSE
    depthStencilDesc2.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK; // デフォルト値
    depthStencilDesc2.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK; // デフォルト値

    pso->SetDsvDesc(depthStencilDesc2);
	pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pso->CreatePSO(int(PSOType::kTransparentTriangle));

    for (int i = 0; i < DrawKindCount; ++i) {
		vertexResource.push_back({});
		wvpResource.push_back({});
		materialResource.push_back({});
		directionalLightResource.push_back({});
		indexResource.push_back({});
        drawCount.push_back(0);
    }

    //SRV用のヒープを作成する
    srvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

}

void MyDirectX::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(myWindow_->GetHwnd(wndHandle_));
    ImGui_ImplDX12_Init(device.Get(),
        2,                                              //swapchainのバッファ数
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,                //色の形式
        srvDescriptorHeap,
        srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
    );
}


void MyDirectX::BeginImGui() {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void MyDirectX::ClearScreen() {
    //これから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //RootSignatureを設定、PSOに設定しているけど別途設定が必要
	SetPSO(PSOType::kOpaqueTriangle);
    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

    //barrierをRenderTargetにする
    InsertBarrier(commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, swapChainResources[backBufferIndex].Get());

    ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);

    //指定した色で画面全体をクリアする
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    //ビューポート
    D3D12_VIEWPORT viewport{};
    //クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = static_cast<float>(kClientWidth);
    viewport.Height = static_cast<float>(kClientHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //シザー矩形
    D3D12_RECT scissorRect{};
    //基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = kClientWidth;
    scissorRect.top = 0;
    scissorRect.bottom = kClientHeight;

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
}

//todo Drawはここだよ
#pragma region Draw系関数

void MyDirectX::DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatirx, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (drawCount[kTriangle] >= vertexResource[kTriangle].size()) {
		assert(false && "over drawcount");
	}

    //頂点リソースにデータを書き込む
    VertexData* vertexData = nullptr;
    //書き込むためのアドレスを取得
    vertexResource[kTriangle][drawCount[kTriangle]]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    //左下
    vertexData[0].position = left;
    vertexData[0].texcoord = { 0.0f, 1.0f };
	vertexData[0].normal = { 0.0f, 0.0f, -1.0f };

    //上
    vertexData[1].position = top;
    vertexData[1].texcoord = { 0.5f, 0.0f };
    vertexData[1].normal = { 0.0f, 0.0f, -1.0f };

    //右下
    vertexData[2].position = right;
    vertexData[2].texcoord = { 1.0f, 1.0f };
    vertexData[2].normal = { 0.0f, 0.0f, -1.0f };

    //データを書き込む
    TramsformMatrixData* wvpData = nullptr;
    //書き込むためのアドレスを取得
    wvpResource[kTriangle][drawCount[kTriangle]]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    //単位行列を書き込む
    wvpData->wvp = wvpMatrix;
	wvpData->world = worldMatirx;

    //データを書き込む
    MaterialData* materialData = nullptr;
    //書き込むためのアドレスを取得
    materialResource[kTriangle][drawCount[kTriangle]]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    
    //光を考慮しない
    *materialData = material;

	//データを書き込む
	DirectionalLightData* directionalLightData = nullptr;
	directionalLightResource[kTriangle][drawCount[kTriangle]]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    *directionalLightData = DirectionalLightData();

    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource[kTriangle][drawCount[kTriangle]]->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    if(material.color.w < 1.0f) {
        //透明な三角形を描画する場合は、PSOを透明用に変更する
        SetPSO(PSOType::kTransparentTriangle);
    } else {
        //不透明な三角形を描画する場合は、PSOを不透明用に変更する
        SetPSO(PSOType::kOpaqueTriangle);
	}

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource[kTriangle][drawCount[kTriangle]]->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource[kTriangle][drawCount[kTriangle]]->GetGPUVirtualAddress());
	//テクスチャの場所を設定
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
	//光のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource[kTriangle][drawCount[kTriangle]]->GetGPUVirtualAddress());
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    commandList->DrawInstanced(3, 1, 0, 0);

    ++drawCount[kTriangle];
}

void MyDirectX::DrawSphere(float radius, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle) {

    if (drawCount[kSphere] >= vertexResource[kSphere].size()) {
        assert(false && "球の描画上限の超過");
    }

    const float pie = 3.14159265358f;
    const int vertical = 32;
    const int horizontal = 16;

    uint32_t drawTriangleCountInstance = 0;

    VertexData* vertexData = nullptr;
    vertexResource[kSphere][drawCount[kSphere]]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    //頂点データを作成する
    for (int i = 0; i < vertical; ++i) {

        int buffer;

        if (i == 0 || i == vertical - 1) {

            float point;

            if (i == 0) {
                point = 0.5f;
                buffer = 1;
            } else {
                point = -0.49f;
                buffer = -1;
            }

            for (int j = 0; j < horizontal; ++j) {
                //球のさきっちょ
                vertexData[drawTriangleCountInstance * 3 + 1].position = { 0.0f, point, 0.0f, 1.0f };
                vertexData[drawTriangleCountInstance * 3 + 1].texcoord = { (float(horizontal - 1 - j)) / float(horizontal - 1), buffer == 1 ? 0.0f : 0.9999f };

                //先っちょから一個離れた点たち
                vertexData[drawTriangleCountInstance * 3 + 1 - buffer].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3 + 1 - buffer].texcoord = {
                    float(horizontal - 1 - j - 1 + (8 * buffer) - 8) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                vertexData[drawTriangleCountInstance * 3 + 1 - buffer * -1].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3 + 1 - buffer * -1].texcoord = {
                    float(horizontal - 1 - j + (8 * buffer) - 8) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                vertexData[drawTriangleCountInstance * 3].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3].position);
                vertexData[drawTriangleCountInstance * 3 + 1].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3 + 1].position);
                vertexData[drawTriangleCountInstance * 3 + 2].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3 + 2].position);

                ++drawTriangleCountInstance;
            }
        } else {

            if (i > vertical / 2) {
                buffer = 1;
            } else {
                buffer = -1;
            }

            for (int j = 0; j < horizontal; ++j) {
                //1つ目の三角形
                //RightBottom
                vertexData[drawTriangleCountInstance * 3 + 2].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3 + 2].texcoord = {
                    float(horizontal - 1 - j) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                //RightTop
                vertexData[drawTriangleCountInstance * 3 + 1].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3 + 1].texcoord = {
                    float(horizontal - 1 - j) / float(horizontal - 1),
                    float(i) / float(vertical)
                };

                //LeftTop
                vertexData[drawTriangleCountInstance * 3].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3].texcoord = {
                    float(horizontal - 1 - j - 1) / float(horizontal - 1),
                    float(i) / float(vertical)
                };

                vertexData[drawTriangleCountInstance * 3].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3].position);
                vertexData[drawTriangleCountInstance * 3 + 1].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3 + 1].position);
                vertexData[drawTriangleCountInstance * 3 + 2].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3 + 2].position);

                ++drawTriangleCountInstance;

                //2つ目の三角形
                //RightBottom
                vertexData[drawTriangleCountInstance * 3 + 2].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3 + 2].texcoord = {
                    float(horizontal - 1 - j) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                //LeftTop
                vertexData[drawTriangleCountInstance * 3 + 1].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3 + 1].texcoord = {
                    float(horizontal - 1 - j - 1) / float(horizontal - 1),
                    float(i) / float(vertical)
                };

                //LeftBottom
                vertexData[drawTriangleCountInstance * 3].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawTriangleCountInstance * 3].texcoord = {
                    float(horizontal - 1 - j - 1) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                vertexData[drawTriangleCountInstance * 3].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3].position);
                vertexData[drawTriangleCountInstance * 3 + 1].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3 + 1].position);
                vertexData[drawTriangleCountInstance * 3 + 2].normal = ConvertVector(vertexData[drawTriangleCountInstance * 3 + 2].position);

                ++drawTriangleCountInstance;
            }

        }
    }

    for (uint32_t i = 0; i < drawTriangleCountInstance * 3; ++i) {
		vertexData[i].position *= radius;
    }

    TramsformMatrixData* wvpData = nullptr;
    //書き込むためのアドレスを取得
    wvpResource[kSphere][drawCount[kSphere]]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    //wvp行列を書き込む
    wvpData->wvp = wvpMatrix;
    wvpData->world = worldMatrix;

    MaterialData* materialData = nullptr;
    //書き込むためのアドレスを取得
    materialResource[kSphere][drawCount[kSphere]]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    //色指定
    *materialData = material;

    DirectionalLightData* directionalLightData = nullptr;
    //書き込むためのアドレスを取得
    directionalLightResource[kSphere][drawCount[kSphere]]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    //光の入力
    *directionalLightData = dLightData;

    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource[kSphere][drawCount[kSphere]]->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 3 * drawTriangleCountInstance;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    if (material.color.w < 1.0f) {
        //透明な三角形を描画する場合は、PSOを透明用に変更する
        SetPSO(PSOType::kTransparentTriangle);
    } else {
        //不透明な三角形を描画する場合は、PSOを不透明用に変更する
        SetPSO(PSOType::kOpaqueTriangle);
    }

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource[kSphere][drawCount[kSphere]]->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource[kSphere][drawCount[kSphere]]->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
    //光のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource[kSphere][drawCount[kSphere]]->GetGPUVirtualAddress());
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    commandList->DrawInstanced(drawTriangleCountInstance * 3, 1, 0, 0);

    ++drawCount[kSphere];
}

void MyDirectX::DrawModel(int modelHandle, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData) {
    uint32_t index = DrawKindCount + modelHandle;

    if (drawCount[index] >= vertexResource[index].size()) {
        assert(false && "over drawcount");
    }

    //頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource[index][drawCount[index]]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelList_[modelHandle].vertices.data(), sizeof(VertexData) * modelList_[modelHandle].vertices.size());

    TramsformMatrixData* wvpData = nullptr;
    //書き込むためのアドレスを取得
    wvpResource[index][drawCount[index]]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    //wvp行列を書き込む
    wvpData->wvp = wvpMatrix;
    wvpData->world = worldMatrix;

    MaterialData* materialData = nullptr;
    //書き込むためのアドレスを取得
    materialResource[index][drawCount[index]]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    //色指定
    *materialData = material;

    DirectionalLightData* directionalLightData = nullptr;
    //書き込むためのアドレスを取得
    directionalLightResource[index][drawCount[index]]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    //光の入力
    *directionalLightData = dLightData;

    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResource[index][drawCount[index]]->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelList_[modelHandle].vertices.size());
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    if (material.color.w < 1.0f) {
        //透明な三角形を描画する場合は、PSOを透明用に変更する
        SetPSO(PSOType::kTransparentTriangle);
    } else {
        //不透明な三角形を描画する場合は、PSOを不透明用に変更する
        SetPSO(PSOType::kOpaqueTriangle);
    }

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource[index][drawCount[index]]->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource[index][drawCount[index]]->GetGPUVirtualAddress());
    //光のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource[index][drawCount[index]]->GetGPUVirtualAddress());
    //テクスチャの設定
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[modelList_[modelHandle].material.textureHandle]);
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawInstanced(UINT(modelList_[modelHandle].vertices.size()), 1, 0, 0);

	drawCount[index]++;

    logger->Log(std::format("{} model drawed", drawCount[index]));
}

void MyDirectX::DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldmat, Matrix4x4 wvpmat, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
    if (drawCount[kSprite] >= vertexResource[kSprite].size()) {
        assert(false && "over drawcount");
    }

    VertexData* vertexData = nullptr;
	vertexResource[kSprite][drawCount[kSprite]]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//左下
    vertexData[0].position = lb;
	vertexData[0].texcoord = { 0.0f, 1.0f };

    //左上
    vertexData[1].position = lt;
    vertexData[1].texcoord = { 0.0f, 0.0f };

	//右下
	vertexData[2].position = rb;
	vertexData[2].texcoord = { 1.0f, 1.0f };

    //右上
    vertexData[3].position = rt;
    vertexData[3].texcoord = { 1.0f, 0.0f };

    uint32_t* indexData = nullptr;
    indexResource[kSprite][drawCount[kSprite]]->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    indexData[3] = 1;
    indexData[4] = 3;
    indexData[5] = 2;

	TramsformMatrixData* matData = nullptr;
	wvpResource[kSprite][drawCount[kSprite]]->Map(0, nullptr, reinterpret_cast<void**>(&matData));
	matData->world = worldmat;
	matData->wvp = wvpmat;

	MaterialData* materialData = nullptr;
	materialResource[kSprite][drawCount[kSprite]]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    *materialData = material;

	DirectionalLightData* directionalLightData = nullptr;
	directionalLightResource[kSprite][drawCount[kSprite]]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	*directionalLightData = dLightData;

    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource[kSprite][drawCount[kSprite]]->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    //インデックスのバッファビューを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	indexBufferView.BufferLocation = indexResource[kSprite][drawCount[kSprite]]->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

    if (material.color.w < 1.0f) {
        //透明な三角形を描画する場合は、PSOを透明用に変更する
        SetPSO(PSOType::kTransparentTriangle);
    } else {
        //不透明な三角形を描画する場合は、PSOを不透明用に変更する
        SetPSO(PSOType::kOpaqueTriangle);
    }

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource[kSprite][drawCount[kSprite]]->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource[kSprite][drawCount[kSprite]]->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
    //光のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource[kSprite][drawCount[kSprite]]->GetGPUVirtualAddress());
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

    ++drawCount[kSprite];
}

void MyDirectX::DrawPrism(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
    if (drawCount[kPrism] >= vertexResource[kPrism].size()) {
        assert(false && "over drawcount");
    }

    VertexData* vertexData = nullptr;
    vertexResource[kPrism][drawCount[kPrism]]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    //上段
    vertexData[0].position = { 0.0f, 1.0f, 0.0f, 1.0f };
    vertexData[0].texcoord = { 0.5f, 0.0f };
    vertexData[0].normal = { 0.0f, 1.0f, 0.0f };

    float pie = 3.14159265358f;

    //中段
    for (int i = 1; i < 6; ++i) {
        vertexData[i].position = { cosf(pie * 2.0f * (i - 1) / 4.0f) / 1.0f, 0.0f, sinf(pie * 2.0f * (i - 1) / 4.0f) / 1.0f, 1.0f };
        vertexData[i].texcoord = { static_cast<float>(i - 1) / 5.0f, 0.5f };
        vertexData[i].normal = { cosf(pie * 2.0f * (i - 1) / 4.0f), 0.0f, sinf(pie * 2.0f * (i - 1) / 4.0f) };
    }

    //下段
    vertexData[6].position = { 0.0f, -1.0f, 0.0f, 1.0f };
    vertexData[6].texcoord = { 0.5f, 1.0f };
    vertexData[6].normal = { 0.0f, -1.0f, 0.0f };

    uint32_t* indexData = nullptr;
    indexResource[kPrism][drawCount[kPrism]]->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

    for (int i = 0; i < 4; ++i) {
        indexData[i * 3] = 0;
        indexData[i * 3 + 1] = i + 2;
        indexData[i * 3 + 2] = i + 1;
    }

    for (int i = 4; i < 8; ++i) {
        indexData[i * 3] = 6;
        indexData[i * 3 + 1] = i + 1 - 4;
        indexData[i * 3 + 2] = i + 2 - 4;
    }

    TramsformMatrixData* wvpData = nullptr;
    wvpResource[kPrism][drawCount[kPrism]]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    wvpData->world = worldMatrix;
    wvpData->wvp = wvpMatrix;

    MaterialData* materialData = nullptr;
    materialResource[kPrism][drawCount[kPrism]]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    *materialData = material;

    DirectionalLightData* directionalLightData = nullptr;
    directionalLightResource[kPrism][drawCount[kPrism]]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    *directionalLightData = dLightData;

    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource[kPrism][drawCount[kPrism]]->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 7;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    //インデックスのバッファビューを作成する
    D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    indexBufferView.BufferLocation = indexResource[kPrism][drawCount[kPrism]]->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = sizeof(uint32_t) * 24;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->IASetIndexBuffer(&indexBufferView);

    if (material.color.w < 1.0f) {
        //透明な三角形を描画する場合は、PSOを透明用に変更する
        SetPSO(PSOType::kTransparentTriangle);
    } else {
        //不透明な三角形を描画する場合は、PSOを不透明用に変更する
        SetPSO(PSOType::kOpaqueTriangle);
    }

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource[kPrism][drawCount[kPrism]]->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource[kPrism][drawCount[kPrism]]->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
    //光のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource[kPrism][drawCount[kPrism]]->GetGPUVirtualAddress());
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    commandList->DrawIndexedInstanced(24, 1, 0, 0, 0);

    ++drawCount[kPrism];
}

void MyDirectX::DrawBox(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
    if (drawCount[kBox] >= vertexResource[kBox].size()) {
        assert(false && "over drawcount");
    }

	VertexData* vertexData = nullptr;
	vertexResource[kBox][drawCount[kBox]]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	uint32_t* indexData = nullptr;
	indexResource[kBox][drawCount[kBox]]->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    
    const Vector2 texcoords[4] = {
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    };

    
    const Vector3 normals[6] = {
        { 0,  0, -1}, 
        { 0,  0,  1}, 
        {-1,  0,  0}, 
        { 1,  0,  0}, 
        { 0,  1,  0}, 
        { 0, -1,  0}  
    };

    const Vector2 faceVerts[4] = {
        {-0.5f, 0.5f}, // 左下
        {0.5f, 0.5f}, // 右下
        {-0.5f, -0.5f}, // 右上
        {0.5f, -0.5f}  // 左上
    };

    int index = 0;
    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < 4; ++i) {
            Vector3 pos;

            float x = faceVerts[i].x;
            float y = faceVerts[i].y;

            switch (face) {
            case 0: pos = { -x, y, -0.5f }; break; // back
            case 1: pos = { x, y,  0.5f }; break; // front
            case 2: pos = { -0.5f, y, x }; break; // left
            case 3: pos = { 0.5f, y, -x }; break; // right
            case 4: pos = { -x,  0.5f, y }; break; // top
            case 5: pos = { x, -0.5f, y }; break; // bottom
            }

            vertexData[index++] = {
                {pos.x, pos.y, pos.z, 1.0f},
                texcoords[i],
                normals[face]
            };
        }
    }

    int idx = 0;
    for (int face = 0; face < 6; ++face) {
        int base = face * 4;
        // 三角形1
        indexData[idx++] = base + 0;
        indexData[idx++] = base + 2;
        indexData[idx++] = base + 3;

        // 三角形2
        indexData[idx++] = base + 0;
        indexData[idx++] = base + 3;
        indexData[idx++] = base + 1;
    }

    TramsformMatrixData* wvpData = nullptr;
    wvpResource[kBox][drawCount[kBox]]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    wvpData->world = worldMatrix;
    wvpData->wvp = wvpMatrix;

    MaterialData* materialData = nullptr;
    materialResource[kBox][drawCount[kBox]]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    *materialData = material;

    DirectionalLightData* directionalLightData = nullptr;
    directionalLightResource[kBox][drawCount[kBox]]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    *directionalLightData = dLightData;

    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource[kBox][drawCount[kBox]]->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 24;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    //インデックスのバッファビューを作成する
    D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    indexBufferView.BufferLocation = indexResource[kBox][drawCount[kBox]]->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = sizeof(uint32_t) * 36;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->IASetIndexBuffer(&indexBufferView);

    if (material.color.w < 1.0f) {
        //透明な三角形を描画する場合は、PSOを透明用に変更する
        SetPSO(PSOType::kTransparentTriangle);
    } else {
        //不透明な三角形を描画する場合は、PSOを不透明用に変更する
        SetPSO(PSOType::kOpaqueTriangle);
    }

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource[kBox][drawCount[kBox]]->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource[kBox][drawCount[kBox]]->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
    //光のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource[kBox][drawCount[kBox]]->GetGPUVirtualAddress());
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

    ++drawCount[kBox];
}

void MyDirectX::DrawLine(Vector4 start, Vector4 end, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle) {

	VertexData* vertexData = nullptr;
	vertexResource[kLine][drawCount[kLine]]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0].position = start;
	vertexData[0].texcoord = { 0.0f, 0.0f };
	vertexData[0].normal = { Normalize({start.x, start.y, start.z}) };

	vertexData[1].position = end;
	vertexData[1].texcoord = { 0.0f, 1.0f };
	vertexData[1].normal = { 0.0f, 0.0f, 0.0f };

	TramsformMatrixData* wvpData = nullptr;
	wvpResource[kLine][drawCount[kLine]]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	wvpData->world = worldMatrix;
	wvpData->wvp = wvpMatrix;

	MaterialData* materialData = nullptr;
	materialResource[kLine][drawCount[kLine]]->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	*materialData = material;

	DirectionalLightData* directionalLightData = nullptr;
	directionalLightResource[kLine][drawCount[kLine]]->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    *directionalLightData = dLightData;

    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource[kLine][drawCount[kLine]]->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 2;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	SetPSO(PSOType::kOpaqueLine);

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource[kLine][drawCount[kLine]]->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource[kLine][drawCount[kLine]]->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
    //光のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource[kLine][drawCount[kLine]]->GetGPUVirtualAddress());
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    commandList->DrawInstanced(2, 1, 0, 0);

    ++drawCount[kLine];
}



#pragma endregion

void MyDirectX::PostDraw() {

    *isCanDraw_ = false;

    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

    InsertBarrier(commandList.Get(), D3D12_RESOURCE_STATE_PRESENT, swapChainResources[backBufferIndex].Get());

    HRESULT hr = commandList->Close();
    assert(SUCCEEDED(hr));


    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);
    //GPUとOSに画面の交換を行うよう通知する
    swapChain->Present(1, 0);

    //Fenceの値を更新
    fenceValue++;
    //GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence.Get(), fenceValue);

    //Fenceの値が指定したSignal値にたどり着いてるかを確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence->GetCompletedValue() < fenceValue) {
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        //イベントを待つ
        WaitForSingleObject(fenceEvent, INFINITE);
    }

	for (uint32_t& count : drawCount) {
		count = 0;
	}

    //フレームの終了をログに通知
	logger->Log(std::format("{} frame ended", ++frame_));
    
    //次のフレームのためのcommandReset
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);

}

void MyDirectX::Finalize() {
    dsvDescriptorHeap->Release();
    depthStencilResource->Release();

    //UnLoad実装時見直し
    for (uint32_t i = 0; i < readTextureCount; ++i) {
        intermediateResource[i]->Release();
        textureResource[i]->Release();
    }

    for (uint32_t i = 0; i < DrawKindCount + modelCount_ + 1; ++i){
        for (uint32_t j = 0; j < vertexResource[i].size(); ++j) {
            vertexResource[i][j]->Release();
            wvpResource[i][j]->Release();
            materialResource[i][j]->Release();
			directionalLightResource[i][j]->Release();
			if (indexResource[i].size() > 0) {
				indexResource[i][j]->Release();
			}
        }
    }
    
    CloseHandle(fenceEvent);
    srvDescriptorHeap->Release();
    rtvDescriptorHeap->Release();

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CoUninitialize();
}

void MyDirectX::InsertBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource* pResource,
    D3D12_RESOURCE_BARRIER_TYPE type, D3D12_RESOURCE_BARRIER_FLAGS flags) {

    D3D12_RESOURCE_STATES stateBefore = resourceStates[pResource];

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = type;
    barrier.Flags = flags;
    barrier.Transition.pResource = pResource;
    barrier.Transition.StateBefore = stateBefore;
    barrier.Transition.StateAfter = stateAfter;

    commandList->ResourceBarrier(1, &barrier);

    // 状態を更新
    resourceStates[pResource] = stateAfter;
}

void MyDirectX::SetPSO(PSOType requirePSO) {
    if (nowPSO == requirePSO) {
        return;
    }

	nowPSO = requirePSO;
    commandList->SetPipelineState(pso->Get(int(requirePSO)));
}
