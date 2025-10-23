#include "OffScreenManager.h"
#include <Core/DXCommonFunction.h>

OffScreenManager::OffScreenManager() {
}

OffScreenManager::~OffScreenManager() {
}

void OffScreenManager::Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager) {
	// OffScreenIndex::MaxCount は enum class 型なので、int にキャストして比較する
	if(maxOffScreenCount_ < static_cast<int>(OffScreenIndex::MaxCount)) {
		assert(0 && "OffScreen num is too large!");
		return;
	}

	device_ = device;
	commandList_ = commandList;
	srvManager_ = srvManager;
	ID3D12DescriptorHeap* rawHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxOffScreenCount_, false);
	rtvDescriptorHeap_.Attach(rawHeap);

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	CreateOffScreen(1280, 720, clearColor); // ImGuiDocking

	clearColor[0] = 1.0f; clearColor[1] = 0.0f; clearColor[2] = 0.0f; clearColor[3] = 1.0f;
	CreateOffScreen(1280, 720, clearColor); // PostPing

	clearColor[0] = 0.0f; clearColor[1] = 0.0f; clearColor[2] = 1.0f; clearColor[3] = 1.0f;
	CreateOffScreen(1280, 720, clearColor); // PostPong

	clearColor[0] = 0.0f; clearColor[1] = 1.0f; clearColor[2] = 0.0f; clearColor[3] = 1.0f;
	CreateOffScreen(1280, 720, clearColor); // GameWindow

	clearColor[0] = 0.3f; clearColor[1] = 0.5f; clearColor[2] = 0.8f; clearColor[3] = 1.0f;
	CreateOffScreen(1280, 720, clearColor); // Title

	for(int i = 5; i < maxOffScreenCount_; ++i) {
		float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		CreateOffScreen(1280, 720, clearColor);
	}
}

int OffScreenManager::CreateOffScreen(int width, int height, float* clearColor) {
	offScreens_.emplace_back(std::make_unique<OffScreenData>(width, height, clearColor, device_, commandList_, srvManager_, rtvDescriptorHeap_.Get()));
	return static_cast<int>(offScreens_.size() - 1);
}

OffScreenData* OffScreenManager::GetOffScreenData(OffScreenIndex index) {
	return offScreens_[int(index)].get();
}
