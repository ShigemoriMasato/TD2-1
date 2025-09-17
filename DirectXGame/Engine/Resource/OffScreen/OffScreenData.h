#pragma once
#include <Core/DXDevice.h>

class OffScreenData {
public:

	/// <summary>
	/// OffScreen用にリソースを作成する
	/// </summary>
	OffScreenData(int width, int height, DXDevice* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srv, ID3D12DescriptorHeap* rtv);

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;
	int width_ = 0;
	int height_ = 0;

	static int offScreenTextureCount_;

	//RTVHandle
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_ = {};

	//バリアの状態
	D3D12_RESOURCE_STATES resourceState_ = D3D12_RESOURCE_STATE_COMMON;
};

