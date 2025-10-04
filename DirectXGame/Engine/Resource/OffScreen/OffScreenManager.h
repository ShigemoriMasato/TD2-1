#pragma once
#include <Core/DXDevice.h>
#include <unordered_map>
#include <string>
#include "OffScreenData.h"
#include <Resource/SRVManager.h>

enum class OffScreenIndex {
	None = -1,
	ImGuiDocking,
	PostEffect,

	MaxCount
};

/// <summary>
/// offscreenの管理クラス。作成時の設定は初期化関数の中身をいじって変えること
/// </summary>
class OffScreenManager {
public:

	OffScreenManager();
	~OffScreenManager();

	/// <summary>
	/// OffScreenを初期化、作成する
	/// </summary>
	void Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager);

	OffScreenData* GetOffScreenData(int handle);

private:

	int CreateOffScreen(int width, int height, float* clearColor);

	std::vector<std::unique_ptr<OffScreenData>> offScreens_;

	DXDevice* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//DescriptorHeap
	SRVManager* srvManager_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

	const int maxOffScreenCount_ = 32;
};

