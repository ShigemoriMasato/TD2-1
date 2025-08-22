#include "GameScene.h"
#include <Input/Input.h>
#include <imgui.h>
#include <Math/MyMath.h>

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()),
debugCamera_(new DebugCamera()),
collisionManager_(std::make_unique<CollisionManager>()) {
	railCameraController_ = std::make_unique<RailCameraController>();
	player_ = std::make_shared<Player>(camera_, railCameraController_->GetCameraPtr(), commonData.get(), railCameraController_->GetSpeedPtr());
	isDebugCamera = false;
	adoveGrid_ = std::make_unique<GridMaker>(camera_, false);
	belowGrid_ = std::make_unique<GridMaker>(camera_, false);
	cometManager_ = std::make_unique<CometManager>(camera_, player_->GetPositionPtr(), commonData.get());
	accelerateGateManager_ = std::make_unique<AccelerateGateManager>(camera_, commonData.get());
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
	adoveGrid_->SetPos({ 0.0f, 10.0f });
	adoveGrid_->Initialize();
	belowGrid_->SetPos({ 0.0f, -20.0f });
	belowGrid_->Initialize();

	AccelerateGate::SetHandle(commonData_->modelHandle_[int(ModelType::AccelerateGate)]);

	accelerateGateManager_->Initialize();

	cometManager_->Initialize();
}

std::unique_ptr<Scene> GameScene::Update() {

	adoveGrid_->Update();
	belowGrid_->Update();

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

	cometManager_->Update();

	accelerateGateManager_->Update();

	AllCollisionCheck();

	return nullptr;
}

void GameScene::Draw() const {
	railCameraController_->Draw(camera_);

	belowGrid_->Draw();

	cometManager_->Draw();

	accelerateGateManager_->Draw();

	player_->Draw();
}

void GameScene::AllCollisionCheck() {

	//Player
	collisionManager_->AddObject(player_->GetCollision());
	//Player's Bullets
	auto bullets = player_->GetBullets();
	for (auto& b : bullets) {
		collisionManager_->AddObject(b->GetCollision());
	}

	//Comets
	auto comets = cometManager_->GetComets();
	for (auto& c : comets) {
		collisionManager_->AddObject(c->GetMainCollision());
		collisionManager_->AddObject(c->GetNearCollision());
	}

	//Gate
	auto gates = accelerateGateManager_->GetGates();
	for (auto& g : gates) {
		collisionManager_->AddObject(g->GetCollision());
	}

	collisionManager_->CheckCollisions();
}
