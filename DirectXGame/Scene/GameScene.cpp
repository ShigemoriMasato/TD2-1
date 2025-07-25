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
	enemies_ = std::make_unique<EnemyManager>(camera_, player_.get(), *commonData_);
	isDebugCamera = true;
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

	enemies_->Initialize();
}

std::unique_ptr<Scene> GameScene::Update() {

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

	enemies_->Update();

	AllCollisionCheck();

	return nullptr;
}

void GameScene::Draw() const {
	player_->Draw();
	
	enemies_->Draw();

	railCameraController_->Draw(camera_);

	Render::DrawModel(commonData_->modelHandle_[int(ModelType::SkySphere)], MakeIdentity4x4(), camera_);
}

void GameScene::AllCollisionCheck() {

	collisionManager_->AddObject(player_.get());
	for (auto& b : player_->GetBullets()) {
		collisionManager_->AddObject(b.get());
	}

	std::list<Object*> enemies = enemies_->GetEnemiesCollition();

	for (const auto& e : enemies) {
		collisionManager_->AddObject(e);
	}

	collisionManager_->CheckCollisions();
}
