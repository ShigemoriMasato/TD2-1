#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"
#include "../GridMaker.h"

class GameScene : public Scene {
public:
	GameScene(std::shared_ptr<CommonData> commonData);
	~GameScene();

	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:

	Camera* camera_;
	DebugCamera* debugCamera_;

	bool isDebugCamera = false;

	std::array<Vector3, size_t(ModelType::ModelCount)> modelPositions_ = {};

	std::unique_ptr<GridMaker> gridMaker_;

	std::unordered_map<ModelType, int> modelHandle;

	MaterialData material_{};
	DirectionalLightData dLight_{};

};
