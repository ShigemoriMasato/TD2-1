#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"
#include "../Player.h"
#include "../CollisionManager.h"
#include "../RailCameraController.h"
#include "../GridMaker.h"
#include "../AccelerateGate.h"
#include "../CometManager.h"

class GameScene : public Scene {
public:
	GameScene(std::shared_ptr<CommonData> commonData);
	~GameScene();

	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:

	void AllCollisionCheck();

	std::unique_ptr<CollisionManager> collisionManager_;

	std::unique_ptr<RailCameraController> railCameraController_ = nullptr;
	Camera* camera_;
	DebugCamera* debugCamera_;
	std::shared_ptr<GridMaker> gridMaker_ = nullptr;

	bool isDebugCamera = false;

	std::shared_ptr<Player> player_ = nullptr;

	std::vector<std::shared_ptr<AccelerateGate>> accelerateGates_;

	std::unique_ptr<CometManager> cometManager_ = nullptr;
};
