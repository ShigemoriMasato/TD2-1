#include "BaseScene.h"
#include <Core/EngineTerminal.h>

void BaseScene::MoveScene(EngineTerminal* engineTerminal, CommonData* commonData) {
	modelManager_ = engineTerminal->GetModelManager();
	textureManager_ = engineTerminal->GetTextureManager();
	render_ = engineTerminal->GetRender();
	input_ = engineTerminal->GetInput();
	fpsObserver_ = engineTerminal->GetFPSObserver();
	this->commonData = commonData;
}
