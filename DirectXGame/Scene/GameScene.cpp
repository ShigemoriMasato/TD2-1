#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()),
debugCamera_(new DebugCamera()),
collisionManager_(std::make_unique<CollisionManager>()) {
	railCameraController_ = std::make_unique<RailCameraController>();
	player_ = std::make_shared<Player>(camera_, railCameraController_->GetCameraPtr(), commonData.get(), railCameraController_->GetSpeedPtr());
	isDebugCamera = false;
	gridMaker_ = std::make_unique<GridMaker>(camera_, false);
}

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	debugCamera_->Initialize();
	player_->Initialize();
	railCameraController_->Initialize();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	gridMaker_->Initialize();

	AccelerateGate::SetHandle(commonData_->modelHandle_[int(ModelType::AccelerateGate)]);

	accelerateGates_.clear();
	std::shared_ptr<AccelerateGate> gate = std::make_shared<AccelerateGate>(camera_, Vector3());
	accelerateGates_.push_back(gate);

	comets_.clear();
	auto comet = std::make_shared<Comet>(camera_,commonData_.get(), player_->GetPositionPtr());
	comet->Initialize();
	comets_.push_back(comet);
}

std::unique_ptr<Scene> GameScene::Update() {

	gridMaker_->Update();

	if (Input::GetKeyState(DIK_R)) {
		Initialize();
	}

	ImGui::Begin("Camera");
	ImGui::Checkbox("Debug Camera", &isDebugCamera);
	ImGui::End();

	railCameraController_->Update();
	*camera_ = railCameraController_->GetCamera();

	if (isDebugCamera) {
		debugCamera_->Update();
		*camera_ = *debugCamera_;
	}

	player_->Update();

	for(auto& c : comets_){
		c->Update();
	}

	AllCollisionCheck();

	return nullptr;
}

void GameScene::Draw() const {
	player_->Draw();
	
	railCameraController_->Draw(camera_);

	gridMaker_->Draw();

	for(auto& a : accelerateGates_) {
		a->Draw();
	}

	for (auto& c : comets_) {
		c->Draw();
	}

}

void GameScene::AllCollisionCheck() {

	collisionManager_->AddObject(player_.get());
	for (auto& b : player_->GetBullets()) {
		collisionManager_->AddObject(b.get());
	}

	for (auto& a : accelerateGates_) {
		collisionManager_->AddObject(a.get());
	}

	collisionManager_->CheckCollisions();
}
