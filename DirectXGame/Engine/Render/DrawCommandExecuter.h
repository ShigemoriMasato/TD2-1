#pragma once
#include "DrawCommandManager.h"
#include <Resource/Data/ModelManager.h>
#include <Resource/Data/TextureManager.h>
#include <Resource/DrawResourceManager.h>
#include <Core/PSO/PSOEditor.h>

class DrawCommandExecuter {
public:

	DrawCommandExecuter(DrawCommandManager* cm, ModelManager* mm, TextureManager* tm, PSOEditor* pe, DrawResourceManager* drm);

	void ExecuteCommands(ID3D12GraphicsCommandList* commandList);

private:

	DrawCommandManager* commandManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	PSOEditor* psoEditor_ = nullptr;
	DrawResourceManager* resourceManager_ = nullptr;

};
