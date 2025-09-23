#include "TitleScene.h"

TitleScene::TitleScene(CommonData* commonData) : BaseScene(commonData) {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
}
