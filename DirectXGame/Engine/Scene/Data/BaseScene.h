#pragma once
#include "CommonData.h"

class EngineTerminal;
class ModelManager;
class TextureManager;
class OffScreenManager;
class Render;
class Input;

class BaseScene {
public:

	BaseScene() = default;
	virtual ~BaseScene() = default;
	virtual void Initialize() = 0;
	virtual std::unique_ptr<BaseScene> Update() = 0;
	virtual void Draw() = 0;
	void MoveScene(EngineTerminal* engineTerminal, CommonData* commonData);

protected:

	CommonData* commonData = nullptr;

	ModelManager* modelManager_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	OffScreenManager* offScreenManager_ = nullptr;
	Render* render_ = nullptr;
	Input* input_ = nullptr;

};

