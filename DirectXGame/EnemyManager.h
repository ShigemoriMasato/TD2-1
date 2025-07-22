#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Scene/Common/CommonData.h"

class EnemyManager {
public:

	EnemyManager(Camera* camera, Object* player, const CommonData& commondata);
	~EnemyManager() = default;

	void Initialize();
	void Update();
	void Draw();

	std::list<Object*> GetEnemiesCollition();

private:

	//CSVを検索して、あるだけ読み込んでvectorにまとめる
	void CreateEnemyQueue();

	void Fire(EnemyBulletDesc desc);

	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::vector<std::unique_ptr<EnemyBullet>> bullets_;

	Object* player_;
	Camera* camera_;

	std::unordered_map<std::string, EnemyInfo> taskQueues_;

	int enemyModelHandle_ = -1; // 敵のモデルハンドル
	int bulletModelHandle_ = -1; // 弾のモデルハンドル
};

