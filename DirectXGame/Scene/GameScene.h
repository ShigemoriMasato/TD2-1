#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"
#include "../GridMaker.h"
#include "../Tools/Binary/BinaryManager.h"

class GameScene : public Scene {
public:
	GameScene(std::shared_ptr<CommonData> commonData);
	~GameScene() override;

	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:

	Camera* persCamera_;
	DebugCamera* debugCamera_;
	Camera* orthoCamera_;

	bool isDebugCamera = false;

	std::array<Vector3, size_t(ModelType::ModelCount)> modelPositions_ = {};
	std::unordered_map<ModelType, int> modelHandle;

	Vector2 uvPos_{};
	Vector2 uvScale_{1.0f, 1.0f};
	float uvRotate_ = 0.0f;

	MaterialData spriteMaterial_{};

	std::unique_ptr<GridMaker> gridMaker_;

	Transform transform_{};

	MaterialData material_{};
	DirectionalLightData dLight_{};

	BinaryManager binaryManager_;

};
