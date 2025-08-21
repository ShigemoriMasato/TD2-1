#pragma once
#include <Data/Transform.h>
#include "Object.h"
#include <functional>
#include <memory>

enum class CollisionType {
	Sphere,
	Capsule,

	CollisionTypeCount
};

class Collision {
public:

	Collision(CollisionType collisionType, Object* object);
	~Collision() = default;

	virtual void Update();

	bool GetIsHit();
	CollisionType GetCollisionType();
	Object* GetObjectPtr();

	virtual void SetCollisionType(CollisionType collisionType);

	std::unique_ptr<Vector3> position_{};

	Sphere sphereConfig_{};
	Capsule capsuleConfig_{};

	std::function<void(Object* other)> onCollision_;

	std::string tag_;

private:

	CollisionType collisionType_{};

	Object* object_;

	bool isHit_{};
	bool isHitMessage_{};

};

class RenderCollision : public Collision, public Object {
public:

	RenderCollision(CollisionType collisionType, Camera* camera, Object* object);
	~RenderCollision() = default;

	void Update() override;

	void SetColor(uint32_t color);

	void SetCollisionType(CollisionType collisionType) override;

private:

};
