#include "TitleScene.h"
#include "../Tools/Input.h"
#include "../externals/imgui/imgui.h"

using namespace Matrix;

TitleScene::TitleScene(CommonData* commonData) : Scene(commonData) {
	camera = new Camera();
	debugCamera = new DebugCamera();
	debugCamera->Initialize();
}

TitleScene::~TitleScene() {
	delete camera;
}

Scene* TitleScene::Update() {

	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
		isDebugCamera = !isDebugCamera; // Toggle camera mode
	}

	ImGui::Begin("Mouse");
	Vector2 mousePos = Input::GetMouseMove();
	ImGui::Text("Mouse Position: (%.2f, %.2f)", mousePos.x, mousePos.y);
	float mouseWheel = Input::GetMouseWheel();
	ImGui::Text("Mouse Wheel: %.2f", mouseWheel);
	ImGui::Text("Mouse Click	%d, %d, %d", Input::GetMouseButtonState()[0], Input::GetMouseButtonState()[2], Input::GetMouseButtonState()[1]);
	ImGui::Text("LSHIFT : %d", Input::GetKeyState()[DIK_LSHIFT]);
	ImGui::End();

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
	
	render_->DrawPrism(MakeIdentity4x4(), camera, { 1.0f, 1.0f, 1.0f, 1.0f, true }, {}, 1);

	debugCamera->Draw(render_, camera);

}
