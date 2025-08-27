#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
	camera_ = std::make_unique<Camera>();
	titleLogo_ = std::make_unique<TitleLogo>(camera_.get(), commonData_->modelHandle_[int(ModelType::Title)]);
	rankBan_ = std::make_unique<RankBan>(commonData_.get());

	auto rankingBoard = RankingBoard();
	ranking_ = rankingBoard.GetRankingList();

	for (int i = 0; i < 10; ++i) {
		numberTextures_[i] = commonData_->textureHandle_[int(TextureType::t0) + i];
	}
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	Transform cameraTransform{};
	cameraTransform.position = { 0.0f, 0.0f, -10.0f };
	camera_->SetTransform(&cameraTransform);
	camera_->MakeMatrix();
	rankBan_->Initialize();
}

std::unique_ptr<Scene> TitleScene::Update() {

	titleLogo_->Update();

	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
		return std::make_unique<GameScene>(commonData_);
	}

	rankBan_->Update();

	return nullptr;
}

void TitleScene::Draw() const {
	titleLogo_->Draw();
	rankBan_->Draw();
}
