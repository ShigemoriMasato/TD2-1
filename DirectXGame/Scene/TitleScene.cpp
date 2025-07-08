#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"
#include "../Engine/Sound/Sound.h"

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
	camera_ = new Camera;
	beatManager_ = new BeatManager(new BPMMeasure());
}

TitleScene::~TitleScene() {
	delete camera_;
}

void TitleScene::Initialize() {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->SetTransform(cameraTrans_);
	cameraTrans_->scale = { 1.0f, 1.0f, 1.0f };
	cameraTrans_->position = { 0.0f, 0.0f, -20.0f };
	camera_->MakeMatrix();
	beatManager_->AddBeatData("TitleBGM", 0, 120); // BGMの追加
	beatManager_->SetUpdateIndex(0); // 最初のBGMを更新対象に設定
	beatManager_->AddBeatData("KakkoiiBGM", 1, 300); // SEの追加
	beatManager_->AddBeatData("IikanjinoBGM", 2, 80); // もう一つのSEの追加
	beatManager_->AddBeatData("MechaikeBGM", 3, 135); // さらにもう一つのSEの追加
}

std::unique_ptr<Scene> TitleScene::Update() {
	beatManager_->Update();
	beatManager_->ImGuiDraw();

	return nullptr;
}

void TitleScene::Draw() const {
	beatManager_->DrawWave(camera_);
}
