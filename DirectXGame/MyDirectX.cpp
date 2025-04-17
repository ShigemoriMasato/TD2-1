#include "MyDirectX.h"
#include <cassert>

MyDirectX::MyDirectX() : dxLog(std::make_unique<Logger>("DirectX12")), fenceEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) {
    Initialize();
}

void MyDirectX::Initialize() {
}

void MyDirectX::ClearWindow(HWND hwnd, uint32_t kClientWidth, uint32_t kClientHeight) {

	CreateSwapChain(hwnd, kClientWidth, kClientHeight);	//スワップチェーンを生成する
	CreateDescriptorHeap();	        //ディスクリプタヒープを生成する
	CreateRenderTargetView();	    //レンダーターゲットビューを生成する
}

void MyDirectX::Finalize() {
}

void MyDirectX::ReleaseChecker() {}



void MyDirectX::CreateD3D12Device() {
}

void MyDirectX::CreateSwapChain(HWND hwnd, uint32_t kClientwidth, uint32_t kClientheight) {
}

void MyDirectX::CreateRenderTargetView() {

}
