#pragma once
#include <Transform/Transform.h>
#include <wrl.h>
#include <cstdint>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include "Data/BaseResource.h"
#include <Resource/OffScreen/OffScreenData.h>

class PostEffectResource : public BaseResource {
public:

	PostEffectResource();
	~PostEffectResource() = default;

	void Initialize();
	
	void DrawReady();

	D3D12_GPU_DESCRIPTOR_HANDLE GetInfoGPUHandle() const { return infoGPUHandle_; }

	//描画したいOffScreenのSRVGPUHandle
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};

private:

	static const int postEffectNum;

	/// <summary>
	/// GPUにポストエフェクトのデータを送るためだけのマジックナンバーだらけの構造体。使うな！！！
	/// </summary>
	struct PostEffectInfo {
		Vector4 color = {};
		float value1 = 0.0f;
		float value2 = 0.0f;
		float value3 = 0.0f;
		int32_t enablePostEffect = 0;
	};

	PostEffectInfo* postEffectInfo_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> infoResource_ = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE infoGPUHandle_{};

};
