#pragma once
#include <Core/DXDevice.h>
#include <Core/PSO/PSOEditor.h>
#include <Render/Resource/DrawResource.h>
#include <Render/Resource/ModelResource.h>
#include <Render/Resource/ParticleResource.h>
#include <Render/Resource/MPResource.h>
#include <Resource/Texture/TextureManager.h>
#include <Resource/OffScreen/OffScreenManager.h>
#include <Render/ImGuiWrapper.h>
#include <imgui/imgui_impl_dx12.h>

class Render {
public:

	Render(DXDevice* device);
	~Render();

	void Initialize(TextureManager* textureManager, OffScreenManager* offScreenManager, SRVManager* srvManager);

	void PreDraw(int offscreenHandle = -1);
	void Draw(DrawResource* resource);
	void Draw(ModelResource* resource);
	void Draw(ParticleResource* resource);
	void Draw(MPResource* resource);
	void PostDraw(ImGuiWrapper* imguiRap);

	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	ImGui_ImplDX12_InitInfo GetImGuiInitInfo(SRVManager* srv);

	void SetClearColor(float r, float g, float b, float a) {
		clearColor_[0] = r;
		clearColor_[1] = g;
		clearColor_[2] = b;
		clearColor_[3] = a;
	}

private:

	void PreDrawSwapChain();
	void PreDrawOffScreen(OffScreenData* offScreen);

	void ResetResourceBarrier();

	//Logger
	std::unique_ptr<Logger> logger_ = nullptr;

	//Device
	DXDevice* device_ = nullptr;

	//Command関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	//RTV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//スワップチェーンの設定
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr, nullptr };
	D3D12_RESOURCE_STATES resourcestates_[2] = { D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_PRESENT };
	float clearColor_[4] = { 0.1f,0.1f,0.1f,1.0f };
	int offScreenHandle_ = -1;			//現在描画対象にしてるOffScreenのハンドル。swapchainは-1

	bool isFrameFirst_ = true;	//PreDrawが初回かどうか

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;
	uint64_t fenceValue;

	//PSO管理
	std::unique_ptr<PSOEditor> psoEditor_ = nullptr;

	//Depth
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;

	//テクスチャ
	TextureManager* textureManager_ = nullptr;
	OffScreenManager* offScreenManager_ = nullptr;

	//SRV
	SRVManager* srvManager_ = nullptr;
};
