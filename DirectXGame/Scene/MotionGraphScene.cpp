#include "MotionGraphScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Sound/Sound.h"
#include "TitleScene.h"

MotionGraphScene::MotionGraphScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
	Sound::bgm[0] = true;
	commonData_->beatManager_->SetUpdateIndex(0);
	commonData_->beatManager_->ToggleUpdate();
	camera_ = std::make_shared<Camera>();
	sampleGraph_ = std::make_unique<SampleGraph>(camera_.get(), commonData_->beatManager_->GetBeatCountPtr());
}

MotionGraphScene::~MotionGraphScene() {
}

void MotionGraphScene::Initialize() {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->SetTransform(&cameraTrans_);

	cameraTrans_.position = { 0.0f, 0.0f, -10.0f };

	camera_->MakeMatrix();

	sampleGraph_->Initialize();
}

std::unique_ptr<Scene> MotionGraphScene::Update() {

	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
		Sound::bgm[0] = false;
		return std::make_unique<TitleScene>(commonData_);
	}

	commonData_->beatManager_->Update();

	sampleGraph_->Update();
	return nullptr;
}

void MotionGraphScene::Draw() const {
	sampleGraph_->Draw();
}
