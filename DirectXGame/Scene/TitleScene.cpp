#include "TitleScene.h"

TitleScene::TitleScene(CommonData* commonData) : Scene(commonData) {
	camera = new Camera();
}

TitleScene::~TitleScene() {
	delete camera;
}

Scene* TitleScene::Update() {

	Transform transform;
	transform.position = { 0.0f, 0.0f, -10.0f };
	transform.rotation = { 0.0f, 0.0f, 0.0f };
	transform.scale = { 1.0f, 1.0f, 1.0f };

	PerspectiveFovDesc perspectiveDesc{};
	camera->SetPerspectiveFovMatrix(perspectiveDesc);
	camera->SetTransform(transform);
	camera->MakeMatrix();

	return nullptr;
}

void TitleScene::Render() {
	
	renderer_->DrawTriangle(
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{0.0f, 0.5f, 0.0f, 1.0f},
		{0.5f, -0.5f, 0.0f, 1.0f},
		MakeIdentity4x4(),
		camera,
		{ 1.0f, 0.0f, 0.0f, 1.0f, true },
		{},
		1
	);

}
