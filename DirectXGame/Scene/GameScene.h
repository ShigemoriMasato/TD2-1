#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"
#include "../Player.h"
#include "../CollisionManager.h"
#include "../RailCameraController.h"
#include "../GridMaker.h"
#include "../AccelerateGatemanager.h"
#include "../CometManager.h"
#include "../ClearMessage.h"
#include "../RankingBoard.h"

class GameScene : public Scene {
public:
	GameScene(std::shared_ptr<CommonData> commonData);
	~GameScene();

	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

	static bool isGoal_;

private:

	void AllCollisionCheck();

	std::unique_ptr<CollisionManager> collisionManager_;

	std::unique_ptr<RailCameraController> railCameraController_ = nullptr;
	Camera* camera_;
	DebugCamera* debugCamera_;
	std::shared_ptr<GridMaker> adoveGrid_ = nullptr;
	std::shared_ptr<GridMaker> belowGrid_ = nullptr;

	bool isDebugCamera = false;

	std::shared_ptr<Player> player_ = nullptr;

	std::unique_ptr<CometManager> cometManager_ = nullptr;

	std::unique_ptr<AccelerateGateManager> accelerateGateManager_ = nullptr;

	std::unique_ptr<ClearMessage> clearMessage_ = nullptr;

	std::unique_ptr<RankingBoard> rankingBoard_ = nullptr;

	bool isPreGoal_ = false;
	long long startTime_ = 0;
};
