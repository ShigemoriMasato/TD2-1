#include "MotionGraphScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Sound/Sound.h"
#include "TitleScene.h"

MotionGraphScene::MotionGraphScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
	Sound::bgm[0] = true;
	commonData_->beatManager_->SetUpdateIndex(0);
	commonData_->beatManager_->ToggleUpdate();
}

MotionGraphScene::~MotionGraphScene() {
}

void MotionGraphScene::Initialize() {
}

std::unique_ptr<Scene> MotionGraphScene::Update() {

	if (Input::GetKeyState(DIK_SPACE) && Input::GetPreKeyState(DIK_SPACE)) {
		return std::make_unique<TitleScene>(commonData_);
	}

	return nullptr;
}

void MotionGraphScene::Draw() const {
}
