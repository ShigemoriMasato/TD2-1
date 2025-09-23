#include "EngineTerminal.h"
#include <Render/Resource/DXResource.h>

EngineTerminal::EngineTerminal() {
}

EngineTerminal::~EngineTerminal() {
}

bool EngineTerminal::IsLoop() {
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else if (msg.message == WM_QUIT) {
		return false;
	}
	return true;
}

void EngineTerminal::Initialize(int32_t windowWidth, int32_t windowHeight) {
	dxDevice_ = std::make_unique<DXDevice>(windowWidth, windowHeight);
	dxDevice_->Initialize();

	DXResource::SetDevice(dxDevice_.get());

	render_ = std::make_unique<Render>(dxDevice_.get());

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxDevice_.get(), render_->GetCommandList());
	modelManager_ = std::make_unique<ModelManager>(textureManager_.get());
	offScreenManager_ = std::make_unique<OffScreenManager>();
	offScreenManager_->Initialize(dxDevice_.get(), render_->GetCommandList());
	input_ = std::make_unique<Input>(nullptr, dxDevice_->GetHwnd());

	render_->Initialize(textureManager_.get(), offScreenManager_.get());
}
