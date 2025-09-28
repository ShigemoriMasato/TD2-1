#include "BaseScene.h"
#include <Core/EngineTerminal.h>

void BaseScene::MoveScene(EngineTerminal* engineTerminal, CommonData* commonData) {
	modelManager_ = engineTerminal->GetModelManager();
	textureManager_ = engineTerminal->GetTextureManager();
	offScreenManager_ = engineTerminal->GetOffScreenManager();
	render_ = engineTerminal->GetRender();
	input_ = engineTerminal->GetInput();
	this->commonData = commonData;
}
