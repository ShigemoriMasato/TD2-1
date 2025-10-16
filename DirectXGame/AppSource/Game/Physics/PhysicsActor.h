#pragma once
#include "Math/MyMath.h"

class PhysicsEngine;
class PhysicsActor
{
	friend class BaseObject;
	friend class PhysicsEngine;

private:

	PhysicsActor(PhysicsEngine* physicsEngine, BaseObject* owner);
	~PhysicsActor();

	void ClearForce() { force_ = { 0.0f,0.0f }; }

	void ResetFlags();

	Vector2 velocity_;

	PhysicsEngine* physicsEngine_ = nullptr;
	BaseObject* owner_ = nullptr;
	Transform* transform_ = nullptr;
	Vector2 force_;
	float mass_ = 1.0f;
	bool useGravity_ = true;
	bool isActive_ = true;


	bool collidedTop_ = false;
	bool collidedBottom_ = false;
	bool collidedLeft_ = false;
	bool collidedRight_ = false;


};

