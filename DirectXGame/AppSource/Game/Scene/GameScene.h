#pragma once
#include "../BaseObject.h"

#include <Game/Camera/CameraManager.h>
#include <Scene/Data/BaseScene.h>
#include <unordered_map>
#include "../Enemy/EnemyManager.h"
#include "../Player/Wire.h"
#include "../Physics/PhysicsEngine.h"
#include "../LevelLoader.h"
#include "../Tile/TileMap.h"
#include "../Goal/GoalTape.h"
#include "../Goal/GoalEvent.h"
#include <Game/FPS/TimeSlower.h>

class Player;
class GameScene : public BaseScene
{
public:

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private://管理用メンバ変数
	//シーン内全てのオブジェクトリスト
	std::vector<std::unique_ptr<BaseObject>> objects_;
	std::unique_ptr<CameraManager> camera_ = nullptr;
	std::unordered_map<Key, bool> keys_{};

	PhysicsEngine physicsEngine_;
	LevelLoader levelLoader_;
	std::unique_ptr<TileMap> tileMap_ = nullptr;
private://パラメータ変数

	std::unique_ptr<TimeSlower> timeSlower_ = nullptr;

private://特定なオブジェクト
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
	Player* player_ = nullptr;
	std::unique_ptr<Wire> wire_ = nullptr;
	GoalTape* goalTape_ = nullptr;

private://イベント
	std::unique_ptr<GoalEvent> goalEvent_ = nullptr;

private://postEffect
	std::unique_ptr<PostEffectResource> postEffect_ = nullptr;
	float goalX_ = 20.0f;		//どこからがゴールか

private://使える関数

void CheckAllCollision();
void CheckPlayerWireField();
};

