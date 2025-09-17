#include "OffScreenManager.h"
#include <Core/DXCommonFunction.h>

OffScreenManager::OffScreenManager(DXDevice* device, ID3D12GraphicsCommandList* commandList) {
	srvDescriptorHeap_ = CreateDescriptorHeap(device->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxOffScreenCount_, true);
	rtvDescriptorHeap_ = CreateDescriptorHeap(device->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxOffScreenCount_, false);
	device_ = device;
	commandList_ = commandList;
}

OffScreenManager::~OffScreenManager() {
}

int OffScreenManager::CreateOffScreen(int width, int height) {
	offScreens_.emplace_back(std::make_unique<OffScreenData>(width, height, device_, commandList_, srvDescriptorHeap_.Get(), rtvDescriptorHeap_.Get()));
	return static_cast<int>(offScreens_.size() - 1);
}

OffScreenData* OffScreenManager::GetOffScreenData(int handle) {
	if (handle < 0 || handle >= static_cast<int>(offScreens_.size())) {
		return nullptr;
	}
	return offScreens_[handle].get();
}
