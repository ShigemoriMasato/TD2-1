#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dbghelp.h>
#include <dxgidebug.h>
#include <memory>
#include "Logger.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")

class MyDirectX {
public:

	MyDirectX();

	void Initialize();

	void ClearWindow(HWND hwnd, uint32_t kClientWidth, uint32_t kClientHeight);

	void Finalize();

	void ReleaseChecker();

private:

	void CreateD3D12Device();

	void CreateSwapChain(HWND hwnd, uint32_t kClientWidth, uint32_t kClientHeight);

	void CreateDescriptorHeap();

	void CreateRenderTargetView();

	std::unique_ptr<Logger> dxLog;	//ログ出力用のポインタ

	HRESULT hr;

	ID3D12Debug1* debugController = nullptr;
	ID3D12InfoQueue* infoQueue = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;
	ID3D12Device* device = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12Resource* swapChainResources[2] = { nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	ID3D12Fence* fence = nullptr;
	HANDLE fenceEvent;
};

