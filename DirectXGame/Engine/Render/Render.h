#pragma once
#include <Core/DXDevice.h>
#include <Core/PSO/PSOEditor.h>
#include <Render/DXResource.h>
#include <Resource/Texture/TextureManager.h>

class Render {
public:

	Render(DXDevice* device);
	~Render();

	void Initialize();

	void PreDraw();
	void Draw(DXResource* resource);

private:

	//Logger
	std::unique_ptr<Logger> logger_ = nullptr;

	//Device
	DXDevice* device_ = nullptr;

	//Command関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	//RTV
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//スワップチェーンの設定
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr, nullptr };

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;
	uint64_t fenceValue;

	//PSO管理
	std::unique_ptr<PSOEditor> psoEditor_ = nullptr;

	//Depth
	ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;
	ID3D12Resource* depthStencilResource = nullptr;

	//Descriptorサイズ
	uint32_t descriptorSizeRTV;

	//テクスチャ
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
};
