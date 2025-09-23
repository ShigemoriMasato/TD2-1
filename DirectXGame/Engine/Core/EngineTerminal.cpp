#include "EngineTerminal.h"

EngineTerminal::EngineTerminal() {
}

EngineTerminal::~EngineTerminal() {
}

void EngineTerminal::Initialize(int32_t windowWidth, int32_t windowHeight) {
	dxDevice_ = std::make_unique<DXDevice>(windowWidth, windowHeight);
	dxDevice_->Initialize();

	render_ = std::make_unique<Render>(dxDevice_.get());

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxDevice_.get(), render_->GetCommandList());
	modelManager_ = std::make_unique<ModelManager>(textureManager_.get());
	offScreenManager_ = std::make_unique<OffScreenManager>();
	offScreenManager_->Initialize(dxDevice_.get(), render_->GetCommandList());
	input_ = std::make_unique<Input>(nullptr, dxDevice_->GetHwnd());

	render_->Initialize(textureManager_.get(), offScreenManager_.get());
}
