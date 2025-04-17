#pragma once
#include <cstdint>
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

};

