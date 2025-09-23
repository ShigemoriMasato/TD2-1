#include "OffScreenManager.h"
#include <Core/DXCommonFunction.h>

OffScreenManager::OffScreenManager() {
}

OffScreenManager::~OffScreenManager() {
}

void OffScreenManager::Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList) {
	device_ = device;
	commandList_ = commandList;
	srvDescriptorHeap_ = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxOffScreenCount_, true);
	rtvDescriptorHeap_ = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxOffScreenCount_, false);
}

int OffScreenManager::CreateOffScreen(int width, int height, float* clearColor) {
	offScreens_.emplace_back(std::make_unique<OffScreenData>(width, height, clearColor, device_, commandList_, srvDescriptorHeap_.Get(), rtvDescriptorHeap_.Get()));
	return static_cast<int>(offScreens_.size() - 1);
}

OffScreenData* OffScreenManager::GetOffScreenData(int handle) {
	if (handle < 0 || handle >= static_cast<int>(offScreens_.size())) {
		return nullptr;
	}
	return offScreens_[handle].get();
}
