#pragma once
#include "Object/Object.h"
#include "EnemyBullet.h"
#include "TimeCall.h"
#include <memory>

class Enemy;

class EnemyState {
public:
	EnemyState(Enemy* enemy) : enemy_(enemy) {}
	virtual std::shared_ptr<EnemyState> Down() = 0;
	virtual std::shared_ptr<EnemyState> Up() = 0;
	virtual void Execute() = 0;
protected:

	Enemy* enemy_;

};

class EnemyStateApploach : public EnemyState {
public:
	EnemyStateApploach(Enemy* enemy) : EnemyState(enemy) {}
	std::shared_ptr<EnemyState> Down() override;
	std::shared_ptr<EnemyState> Up() override;
	void Execute() override;

private:



};

class EnemyStateLeave : public EnemyState {
public:
	EnemyStateLeave(Enemy* enemy) : EnemyState(enemy) {}
	std::shared_ptr<EnemyState> Down() override;
	std::shared_ptr<EnemyState> Up() override;
	void Execute() override;

private:

};

class Enemy : public Object {
public:
	Enemy(Camera* camera, int modelHandle, int bulletHandle, Object* target);
	~Enemy() = default;

	void Initialize() override;

	void Update() override;

	std::shared_ptr<EnemyState> Down();
	std::shared_ptr<EnemyState> Up();

	/// <summary>
	/// EnemyBulletとEnemyのDraw
	/// </summary>
	void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

	void MovePosition(Vector3 velocity);

	bool GetIsAlive() const { return isActive_; }

	void OnCollision(Object* object) override;

	std::vector<std::shared_ptr<EnemyBullet>> GetBullets() const { return bullets_; }

private:

	void Fire();

	void Death() { isActive_ = false; }

	using StateFunction = void (Enemy::*)(); // メンバ関数ポインタ型を定義
	static StateFunction stateFunc[];       // メンバ関数ポインタの配列を宣言

	std::shared_ptr<EnemyState> state_;
	
	int frame_ = 0;

	//弾
	int bulletModelHandle_ = 0;
	std::vector<std::shared_ptr<EnemyBullet>> bullets_;
	int fireCooltime_ = 0;
	static inline const int fireCooltimeMax = 60;

	//TimeCall
	TimeCall* timecall_ = nullptr;

	Object* target_ = nullptr; // プレイヤーへの参照
};

