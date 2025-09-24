#pragma once
#include <Core/DXDevice.h>
#include <unordered_map>
#include <string>
#include "OffScreenData.h"
#include <Resource/SRVManager.h>

class OffScreenManager {
public:

	OffScreenManager();
	~OffScreenManager();

	void Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager);

	int CreateOffScreen(int width, int height, float* clearColor);
	OffScreenData* GetOffScreenData(int handle);

private:
	
	std::vector<std::unique_ptr<OffScreenData>> offScreens_;

	DXDevice* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//DescriptorHeap
	SRVManager* srvManager_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

	const int maxOffScreenCount_ = 32;
};

