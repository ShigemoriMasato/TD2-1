#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <sstream>
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Scene/Common/CommonData.h"

struct StageInfo {
	//ステージの名前
	std::string name;

	//敵の行動キュー(初期化時に解読する)
	std::vector<EnemyCommand> queue;

	//敵の行動キューがどの行に記述されていたか(行動の削除など、他の行動に干渉する際に使用)
	std::vector<int> queueLines;
};

class EnemyManager {
public:

	EnemyManager(Camera* camera, Object* player, const CommonData& commondata);
	~EnemyManager() = default;

	void Initialize();
	void Update();
	void Draw();

	std::list<Object*> GetEnemiesAndBulletCollition();
	std::list<Enemy*> GetEnemiesCollition();

private:

	//CSVを検索して、あるだけ読み込んでvectorにまとめる
	void CreateEnemyQueue();
	void CreateStageQueue();

	void UpdateStageQueue(std::string stageName);

	void Fire(EnemyBulletDesc desc);

	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::vector<std::unique_ptr<EnemyBullet>> bullets_;

	Object* player_;
	Camera* camera_;

	std::unordered_map<std::string, EnemyInfo> taskQueues_;
	std::unordered_map<std::string, std::stringstream> stageQueues_;

	int enemyModelHandle_ = -1; // 敵のモデルハンドル
	int bulletModelHandle_ = -1; // 弾のモデルハンドル

	bool waiting_ = false;
	int waitTime_ = 0; // 待機時間
};

