#include "TitleScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"

using namespace Matrix;

TitleScene::TitleScene(CommonData* commonData) : Scene(commonData) {
	camera = new Camera();
	debugCamera = new DebugCamera();
	player_ = new Player();
	player_->Initialize(camera);
	debugCamera->Initialize();
}

TitleScene::~TitleScene() {
	delete camera;
	delete player_;
}

Scene* TitleScene::Update() {

	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
		isDebugCamera = !isDebugCamera; // Toggle camera mode
	}

	player_->Update();

	if (isDebugCamera) {
		debugCamera->Update();
		*camera = debugCamera->GetCamera();
	} else {
		Transform transform;
		transform.position = { 0.0f, 0.0f, -10.0f };
		transform.rotation = { 0.0f, 0.0f, 0.0f };
		transform.scale = { 1.0f, 1.0f, 1.0f };

		camera->SetTransform(transform);
		camera->SetProjectionMatrix(PerspectiveFovDesc());
	}
	camera->MakeMatrix();

	return nullptr;
}

void TitleScene::Draw() const {
	player_->Draw();
}
