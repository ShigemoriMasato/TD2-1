#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "EnemyBullet.h"

class EnemyManager {
public:

	EnemyManager(Camera* camera, Object* player);
	~EnemyManager() = default;

	void Initialize();
	void Update();
	void Draw();

	void AddEnemyBullet(std::unique_ptr<Enemy> enemy, EnemyBulletDesc desc);

private:

	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::vector<std::unique_ptr<EnemyBullet>> bullets_;

	Object* player_;
	Camera* camera_;
};

