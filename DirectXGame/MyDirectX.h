#pragma once
#include <Windows.h>
#include <cstdint>
#include <cassert>
#include <vector>
#include <strsafe.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dbghelp.h>
#include <dxgidebug.h>
#include <memory>
#include <dxcapi.h>
#include <unordered_map>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
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

		MaxDrawKind
	};

	MyDirectX(int32_t kWindowWidth, int32_t kWindowHeight);
	~MyDirectX();

	void Initialize();

	void CreateDrawResource(DrawKind drawKind, uint32_t createNum);

	void BeginFrame();

	int ReadTexture(std::string path);

	void DrawTriangle3D(Vector4 left, Vector4 top, Vector4 right, Vector4 color, int textureHandle);

	void DrawTriangle(TriangleData3 vertexData, Vector4 color, int textureHandle);

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

	uint32_t drawTriangle3DCount = 0;

	std::vector<std::vector<ID3D12Resource*>> vertexResource;
	std::vector<std::vector<ID3D12Resource*>> wvpResource;
	std::vector<std::vector<ID3D12Resource*>> materialResource;

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

