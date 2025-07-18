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
	player_ = std::make_shared<Player>(camera_, railCameraController_->GetCameraPtr(),
		commonData->modelHandle_[int(ModelType::Player)], commonData_->modelHandle_[int(ModelType::Bullet)]);
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

	enemies_.clear();
	CreateEnemy();
}

std::unique_ptr<Scene> GameScene::Update() {

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

	for (int i = 0; i < enemies_.size(); ++i) {
		enemies_[i]->Update();
		if (!enemies_[i]->GetIsAlive()) {
			enemies_.erase(enemies_.begin() + i);
			//新しい敵を生成
			CreateEnemy();
		}
	}

	AllCollisionCheck();

	return nullptr;
}

void GameScene::Draw() const {
	player_->Draw();
	
	for (const auto& enemy : enemies_) {
		enemy->Draw();
	}

	railCameraController_->Draw(camera_);

	Render::DrawModel(commonData_->modelHandle_[int(ModelType::SkySphere)], MakeIdentity4x4(), camera_);
}

void GameScene::CreateEnemy() {
	std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(camera_, commonData_->modelHandle_[int(ModelType::Enemy)], commonData_->modelHandle_[int(ModelType::Bullet)],  player_.get());
	enemy->Initialize();
	enemies_.push_back(enemy);
}

void GameScene::AllCollisionCheck() {

	collisionManager_->AddObject(player_.get());
	for (auto& b : player_->GetBullets()) {
		collisionManager_->AddObject(b.get());
	}

	for(const auto& enemy : enemies_) {
		collisionManager_->AddObject(enemy.get());
	}
	for (const auto& e : enemies_) {
		for (const auto& b : e->GetBullets()) {
			collisionManager_->AddObject(b.get());
		}
	}

	collisionManager_->CheckCollisions();
}
