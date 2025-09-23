#include "BaseScene.h"

void BaseScene::MoveScene(EngineTerminal* engineTerminal) {
	modelManager_ = engineTerminal->GetModelManager();
	textureManager_ = engineTerminal->GetTextureManager();
	offScreenManager_ = engineTerminal->GetOffScreenManager();
	render_ = engineTerminal->GetRender();
	input_ = engineTerminal->GetInput();
}
