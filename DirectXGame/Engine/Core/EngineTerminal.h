#pragma once
#include <Core/DXDevice.h>
#include <Render/Render.h>
#include <Resource/Texture/TextureManager.h>
#include <Resource/Model/ModelManager.h>
#include <Resource/OffScreen/OffScreenManager.h>
#include <Render/ImGuiWrapper.h>
#include <Input/Input.h>

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

class EngineTerminal {
public:

	EngineTerminal();
	~EngineTerminal();
	void Initialize(int32_t windowWidth, int32_t windowHeight);

	bool IsLoop();

	void Update();

	void PreDraw();
	void PostDraw();

	DXDevice* GetDXDevice() { return dxDevice_.get(); }
	Render* GetRender() { return render_.get(); }
	TextureManager* GetTextureManager() { return textureManager_.get(); }
	ModelManager* GetModelManager() { return modelManager_.get(); }
	OffScreenManager* GetOffScreenManager() { return offScreenManager_.get(); }
	Input* GetInput() { return input_.get(); }

private:

	D3DResourceLeakChecker leakChecker_;
	std::unique_ptr<DXDevice> dxDevice_ = nullptr;
	std::unique_ptr<Render> render_ = nullptr;
	std::unique_ptr<SRVManager> srvManager_ = nullptr;
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
	std::unique_ptr<ModelManager> modelManager_ = nullptr;
	std::unique_ptr<OffScreenManager> offScreenManager_ = nullptr;
	std::unique_ptr<Input> input_ = nullptr;
	std::unique_ptr<ImGuiRapper> imgui_ = nullptr;

	MSG msg{};

};

