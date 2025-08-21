#pragma once
#include "Object/Object.h"
#include "Object/Collision.h"

class PlayerBullet : public Object {
public:

	PlayerBullet(Camera* camera, Vector3 pos, Vector3 target, int handle);
	~PlayerBullet() = default;
	void Initialize();
	void Update();

	void OnCollision(Object* other) override;

	Collision* GetCollision() { return collision_.get(); }

private:

	Vector3 velocity_;
	Vector3 direction_;

	std::shared_ptr<Collision> collision_;

	int frame_ = 0;

	static inline const float speed = 1.0f;
};

