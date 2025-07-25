#pragma once
#include <Windows.h>
#include <unordered_map>
#include <vector>
#include "../../externals/imgui/imgui_impl_win32.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dbghelp.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>

#include "../Logger/Logger.h"
#include "../Math/MyMath.h"
#include "../Sound/Audio.h"
#include "../Core/MyWindow.h"
#include "../Core/MyPSO.h"
#define WHIETE1x1 2
#define UVCHECKER 1

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

//必要な時に必要なクラスを作成して、ここにポインタを集中させるようにする

class MyDirectX {
public:
	enum DrawKind {
		kTriangle,
		kSphere,
		kSprite,
		kPrism,
		kBox,
		kLine,

		DrawKindCount
	};

	MyDirectX(int32_t kWindowWidth, int32_t kWindowHeight);
	~MyDirectX();

	void Initialize();

	int CreateDrawResource(DrawKind drawKind, uint32_t createNum);
	int CreateModelDrawResource(uint32_t modelHandle, uint32_t createNum);

	void BeginFrame();

	void PreDraw();

	int LoadTexture(std::string path);

	int LoadObjFile(const std::string& directoryPath, const std::string& filename);

	void DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatirx, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawSphere(float radius, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawModel(int modelHandle, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData);

	void DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldmat, Matrix4x4 wvpmat, MaterialData material, DirectionalLightData dLightData, int textureHandle, bool isOffScreen = false);

	void DrawPrism(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawBox(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawLine(Vector4 start, Vector4 end, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void PostDraw();

	void Finalize();

	HWND GetMyHwnd() const { return myWindow_->GetHwnd(wndHandle_); }

	WNDCLASS GetMyWndClass() const { return myWindow_->GetWndClass(wndHandle_); }

	bool* GetIsCanDraw() const { return isCanDraw_; }

private:

	std::vector<ModelMaterial> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	enum class PSOType {
		kUnknown = -1,
		kOpaqueTriangle,		//不透明三角形
		kTransparentTriangle,	//透明三角形

		kOpaqueLine,			//不透明線

		kOffScreen,

		PSOTypeCount
	};

	void ClearScreen();

	void BeginImGui();
	void InitDirectX();
	void InitImGui();

	void InsertBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource* pResource,
		D3D12_RESOURCE_BARRIER_TYPE type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

	void SetPSO(PSOType requirePSO);

	void Draw(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle, MyDirectX::DrawKind kind, MyDirectX::PSOType pso, VertexData* vertexData, int vertexNum, uint32_t* index = nullptr, int indexNum = -1);

	Logger* logger;

	const int32_t kClientWidth;		//ウィンドウ幅
	const int32_t kClientHeight;	//ウィンドウ高さ
	float* clearColor;		//windowの色

	MyWindow* myWindow_ = nullptr;
	int wndHandle_;

	//DirectXCommon
	D3DResourceLeakChecker leakChecker;
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	uint64_t fenceValue;


	//スワップチェーンの設定
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr, nullptr };
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;

	//三角形描画用
	ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;
	ID3D12Resource* depthStencilResource = nullptr;
	MyPSO* pso = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = nullptr;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	//1フレームに描画した数をカウントする
	std::vector<uint32_t> drawCount;

	std::vector<std::vector<ID3D12Resource*>> vertexResource;
	std::vector<std::vector<ID3D12Resource*>> wvpResource;
	std::vector<std::vector<ID3D12Resource*>> materialResource;
	std::vector<std::vector<ID3D12Resource*>> directionalLightResource;
	std::vector<std::vector<ID3D12Resource*>> indexResource;

	//Model管理用
	std::vector<ModelData> modelList_;
	uint32_t modelCount_ = -1; //モデルの数

	//画像の関数
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandleGPU;
	uint32_t readTextureCount;
	std::vector<ID3D12Resource*> textureResource;
	std::vector<ID3D12Resource*> intermediateResource;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	//imgui用
	ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[3];

	std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates;

	bool* isCanDraw_ = nullptr; //描画可能かどうかのフラグ

	uint32_t frame_ = 0; //フレーム数

	//PSO管理用
	PSOType nowPSO = PSOType::kUnknown;
};

