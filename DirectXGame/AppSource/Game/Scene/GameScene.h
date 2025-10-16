#pragma once
#include "../BaseObject.h"

#include <Camera/DebugCamera.h>
#include <Scene/Data/BaseScene.h>
#include <unordered_map>
#include "../Enemy/EnemyManager.h"
#include "../Player/Wire.h"
#include "../Physics/PhysicsEngine.h"
#include "../LevelLoader.h"
#include "../Tile/TileMap.h"
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
	std::unique_ptr<DebugCamera> camera_ = nullptr;
	std::unordered_map<Key, bool> keys_{};

	PhysicsEngine physicsEngine_;
	LevelLoader levelLoader_;
	std::unique_ptr<TileMap> tileMap_ = nullptr;
private://パラメータ変数

	std::unique_ptr<TimeSlower> timeSlower_ = nullptr;
	bool isInWireField_ = false;

private://特定なオブジェクト
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
	Player* player_ = nullptr;
	std::unique_ptr<Wire> wire_ = nullptr;
private://使える関数

void CheckAllCollision();
void CheckPlayerWireField();
///// @brief 敵の配置を設定する
//void SetupEnemies();
};

