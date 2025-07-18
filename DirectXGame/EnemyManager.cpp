#include "EnemyManager.h"

EnemyManager::EnemyManager(Camera* camera, Object* player) {
	player_ = player;
	camera_ = camera;
}

void EnemyManager::Initialize() {
	enemies_.clear();
	bullets_.clear();
}

void EnemyManager::Update() {
	for (int i = 0; i < enemies_.size(); ++i) {
		enemies_[i]->Update();
		if (!enemies_[i]->GetIsAlive()) {
			enemies_.erase(enemies_.begin() + i--);
		}
	}

	for (int i = 0; i < bullets_.size(); ++i) {
		bullets_[i]->Update();
		if (!bullets_[i]->GetIsActive()) {
			bullets_.erase(bullets_.begin() + i--);
		}
	}
}

void EnemyManager::Draw() {
	for (const auto& enemy : enemies_) {
		enemy->Draw();
	}
	for (const auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void EnemyManager::AddEnemyBullet(std::unique_ptr<Enemy> enemy, EnemyBulletDesc desc) {
	std::unique_ptr<EnemyBullet> bullet = 
		std::make_unique<EnemyBullet>(camera_, desc.position, desc.modelHandle, desc.target);

	bullet->Initialize();
	bullets_.push_back(std::move(bullet));
}
