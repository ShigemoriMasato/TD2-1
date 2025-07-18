#include "Enemy.h"
#include <numbers>

Enemy::Enemy(Camera* camera, int modelHandle, int bulletHandle, Object* target) : Object(camera, ShapeType::Model),
timecall_(new TimeCall()),
target_(target) {
	handle_ = modelHandle;
	bulletModelHandle_ = bulletHandle;
	tag = "Enemy";
}

void Enemy::Initialize() {
	transform_->position = { 1.0f, 0.0f, 5.0f };
	state_ = std::make_shared<EnemyStateApploach>(this);
	fireCooltime_ = 0;
	timecall_->Register(std::bind(&Enemy::Fire, this), 10, true);
	timecall_->Register(std::bind(&Enemy::Death, this), 0xfffffff, false);
}

void Enemy::Update() {
	state_->Execute();

	timecall_->Update();

	for (int i = 0; i < bullets_.size(); ++i) {
		bullets_[i]->Update();
		if (!bullets_[i]->GetIsActive()) {
			bullets_.erase(bullets_.begin() + i--);
		}
	}
}

void Enemy::Draw(const Matrix4x4* worldMatrix) const {
	Object::Draw();

	for (const auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Enemy::OnCollision(Object* other) {
	Down();
}

void Enemy::Fire() {
	std::shared_ptr<EnemyBullet> bullet = std::make_shared<EnemyBullet>(camera_, transform_->position, bulletModelHandle_, target_);
	bullet->Initialize();
	bullets_.push_back(bullet);
}

std::shared_ptr<EnemyState> Enemy::Down() {
	return state_->Down();
}

std::shared_ptr<EnemyState> Enemy::Up() {
	return state_->Up();
}

void Enemy::MovePosition(Vector3 velocity) {
	transform_->position += velocity;
}

std::shared_ptr<EnemyState> EnemyStateApploach::Down() {
	return std::make_shared<EnemyStateLeave>(enemy_);
}

std::shared_ptr<EnemyState> EnemyStateApploach::Up() {
	return std::shared_ptr<EnemyState>(this); // 変化なし
}

void EnemyStateApploach::Execute() {
	enemy_->MovePosition(Vector3(0.0f, 0.0f, -0.05f));
}

std::shared_ptr<EnemyState> EnemyStateLeave::Down() {
	return std::shared_ptr<EnemyState>(this); // 変化なし
}

std::shared_ptr<EnemyState> EnemyStateLeave::Up() {
	return std::make_shared<EnemyStateApploach>(enemy_);
}

void EnemyStateLeave::Execute() {
	enemy_->MovePosition(Vector3(0.05f, 0.05f, 0.05f));
}
