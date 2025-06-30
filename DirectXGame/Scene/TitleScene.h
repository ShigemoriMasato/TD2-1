#pragma once
#include "Common/Scene.h"
#include "../Engine/Camera/DebugCamera.h"
#include <array>

class TitleScene : public Scene {
public:

	TitleScene(std::shared_ptr<CommonData> commonData);
	~TitleScene();
	void Initialize() override;
	Scene* Update() override;
	void Draw() const override;

private:
	Camera* camera_ = nullptr;
	Transform cameraTransform_;

	Vector3 pos;

	std::array<std::array<Vector3, 32>, 2> start_;
	std::array<std::array<Vector3, 32>, 2> end_;

	Transform transform_;
	Vector3 spherical_ = {};
	float speed_ = 0.01f;
};
