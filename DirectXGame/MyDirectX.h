#pragma once
#include <Windows.h>
#include <unordered_map>
#include <vector>
#include "externals/imgui/imgui_impl_win32.h"

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include "Logger.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MyMath.h"
#include "VertexData.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class MyDirectX {
public:
	enum DrawKind {
		kTriangle3D,
		kTriangle2D,
		kSphere,
		kSprite3D,
		kSprite2D,
		kPrism,

		MaxDrawKind
	};

	MyDirectX(int32_t kWindowWidth, int32_t kWindowHeight);
	~MyDirectX();

	void Initialize();

	void CreateDrawResource(DrawKind drawKind, uint32_t createNum);

	void BeginFrame();

	int ReadTexture(std::string path);

	void DrawTriangle3D(Vector4 left, Vector4 top, Vector4 right, Vector4 color, DirectionalLightData dLightData, int textureHandle);

	void DrawTriangle(TriangleData3 vertexData, Vector4 color, DirectionalLightData dLightData, int textureHandle);

	void DrawSphere(Vector4 center, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawSprite3D(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 wvpmat, Matrix4x4 worldmat, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawPrism(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void EndFrame();

	void Finalize();

private:

	void ClearScreen();

	void BeginImGui();

	void CreateWindowForApp();

	void InitDirectX();

	void InitImGui();

	void InsertBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource* pResource,
		D3D12_RESOURCE_BARRIER_TYPE type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

	Logger* logger;

	const int32_t kClientWidth;		//ウィンドウ幅
	const int32_t kClientHeight;	//ウィンドウ高さ
	float* clearColor;		//windowの色

	HWND hwnd;

	//DirectXCommon
	ID3D12Debug1* debugController = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;
	ID3D12Device* device = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	uint64_t fenceValue;


	//スワップチェーンの設定
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12Resource* swapChainResources[2] = { nullptr, nullptr };
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	ID3D12Fence* fence = nullptr;
	HANDLE fenceEvent;

	//三角形描画用
	ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;
	ID3D12Resource* depthStencilResource = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	ID3D10Blob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	IDxcBlob* pixelShaderBlob = nullptr;
	IDxcBlob* vertexShaderBlob = nullptr;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	//1フレームに描画した数をカウントする
	std::vector<uint32_t> drawCount;
	uint32_t drawTriangle3DCount = 0;
	uint32_t drawSphereCount = 0;

	std::vector<std::vector<ID3D12Resource*>> vertexResource;
	std::vector<std::vector<ID3D12Resource*>> wvpResource;
	std::vector<std::vector<ID3D12Resource*>> materialResource;
	std::vector<std::vector<ID3D12Resource*>> directionalLightResource;
	std::vector<std::vector<ID3D12Resource*>> indexResource;

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

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates;
};

