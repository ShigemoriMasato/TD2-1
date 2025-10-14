#pragma once
#include "../BaseObject.h"

#include <Camera/DebugCamera.h>
#include <Scene/Data/BaseScene.h>
#include <unordered_map>
#include "../Enemy/EnemyManager.h"
#include "../Player/Wire.h"

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
private://パラメータ変数

	float slowmotionFactor_ = 1.0f;
	bool isInWireField_ = false;

private://特定なオブジェクト
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
	Player* player_ = nullptr;
	std::unique_ptr<Wire> wire_ = nullptr;
private://使える関数

	void CheckAllCollision();
	void CheckPlayerWireField();
};

