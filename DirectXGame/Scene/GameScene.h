#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"
#include "../Player.h"
#include "../EnemyManager.h"
#include "../CollisionManager.h"
#include "../RailCameraController.h"

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

	bool isDebugCamera = false;

	std::shared_ptr<Player> player_ = nullptr;

	std::unique_ptr<EnemyManager> enemies_ = nullptr;
};
