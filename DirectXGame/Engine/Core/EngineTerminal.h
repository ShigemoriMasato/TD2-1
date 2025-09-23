#pragma once
#include <Core/DXDevice.h>
#include <Render/Render.h>
#include <Resource/Texture/TextureManager.h>
#include <Resource/Model/ModelManager.h>
#include <Resource/OffScreen/OffScreenManager.h>
#include <Input/Input.h>

class EngineTerminal {
public:

	EngineTerminal();
	~EngineTerminal();
	void Initialize(int32_t windowWidth, int32_t windowHeight);
	DXDevice* GetDXDevice() { return dxDevice_.get(); }
	Render* GetRender() { return render_.get(); }
	TextureManager* GetTextureManager() { return textureManager_.get(); }
	ModelManager* GetModelManager() { return modelManager_.get(); }
	OffScreenManager* GetOffScreenManager() { return offScreenManager_.get(); }
	Input* GetInput() { return input_.get(); }

private:

	std::unique_ptr<DXDevice> dxDevice_ = nullptr;
	std::unique_ptr<Render> render_ = nullptr;
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
	std::unique_ptr<ModelManager> modelManager_ = nullptr;
	std::unique_ptr<OffScreenManager> offScreenManager_ = nullptr;
	std::unique_ptr<Input> input_ = nullptr;

};

