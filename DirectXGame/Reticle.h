#pragma once
#include "EnemyManager.h"

class Player;

class Reticle {
public:

	Reticle(Player* player);
	~Reticle() = default;

	void GetTarget(std::list<Enemy*> enemies);

private:

	Player* player_ = nullptr; // プレイヤーへの参照

};
