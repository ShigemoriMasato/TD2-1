#pragma once
#include <Core/DXDevice.h>
#include <unordered_map>
#include <string>
#include "OffScreenData.h"

class OffScreenManager {
public:

	OffScreenManager(DXDevice* device, ID3D12GraphicsCommandList* commandList);
	~OffScreenManager();

	int CreateOffScreen(int width, int height);
	OffScreenData* GetOffScreenData(int handle);

private:
	
	std::vector<std::unique_ptr<OffScreenData>> offScreens_;

	DXDevice* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//SRVHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

	const int maxOffScreenCount_ = 32;
};

