#pragma once
#include "Object/Object.h"

struct EnemyBulletDesc {
	Vector3 position = {};
	Vector3 direction = { 0.0f, 0.0f, 1.0f };
	float speed = 0.1f;
	uint32_t color = 0xff0000ff;
	bool isTracking = false;
};

class EnemyBullet : public Object {
public:

	EnemyBullet(Camera* camera, Vector3 pos, int handle, Object* target);
	~EnemyBullet() = default;
	void Initialize() override;
	void Update() override;

	void OnCollision(Object* other) override;

private:

	Vector3 velocity_;
	Vector3 direction_;
	static inline const float speed = 0.2f;

	int frame_ = 0;

	Object* target_ = nullptr;
};

