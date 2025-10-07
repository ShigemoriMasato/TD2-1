#pragma once
#include "CommonData.h"
#include <Core/EngineTerminal.h>

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
	Render* render_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audio_ = nullptr;

};

