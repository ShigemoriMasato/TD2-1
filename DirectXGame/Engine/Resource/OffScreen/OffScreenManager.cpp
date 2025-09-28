#include "OffScreenManager.h"
#include <Core/DXCommonFunction.h>

OffScreenManager::OffScreenManager() {
}

OffScreenManager::~OffScreenManager() {
}

void OffScreenManager::Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager) {
	device_ = device;
	commandList_ = commandList;
	srvManager_ = srvManager;
	ID3D12DescriptorHeap* rawHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxOffScreenCount_, false);
	rtvDescriptorHeap_.Attach(rawHeap);
}

int OffScreenManager::CreateOffScreen(int width, int height, float* clearColor) {
	offScreens_.emplace_back(std::make_unique<OffScreenData>(width, height, clearColor, device_, commandList_, srvManager_, rtvDescriptorHeap_.Get()));
	return static_cast<int>(offScreens_.size() - 1);
}

OffScreenData* OffScreenManager::GetOffScreenData(int handle) {
	if (handle < 0 || handle >= static_cast<int>(offScreens_.size())) {
		return nullptr;
	}
	return offScreens_[handle].get();
}
