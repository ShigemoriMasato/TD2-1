#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <memory>
#include <Logger/Logger.h>
#include "MyWindow.h"

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

/// <summary>
/// Window呼び出しとDevice作成
/// </summary>
class DXDevice {
public:

	DXDevice(uint32_t windowWidth, uint32_t windowHeight);
	~DXDevice();

	void Initialize();

	void SetWindowProc(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> windowProc);

private:

	void CreateDevice();

	D3DResourceLeakChecker leakChecker;
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	std::unique_ptr<MyWindow> window_ = nullptr;

	std::unique_ptr<Logger> logger = nullptr;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

};

