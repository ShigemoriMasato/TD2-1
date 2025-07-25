#pragma once
#include "Object/Object.h"

class PlayerBullet : public Object {
public:

	PlayerBullet(Camera* camera, Vector3 pos, Vector3 rotate, int handle);
	~PlayerBullet() = default;
	void Initialize() override;
	void Update() override;

	void OnCollision(Object* other) override;

private:

	Vector3 velocity_;
	Vector3 direction_;

	int frame_ = 0;

	static inline const float speed = 0.2f;
};

