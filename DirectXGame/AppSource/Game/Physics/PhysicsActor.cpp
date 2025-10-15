#include "PhysicsActor.h"
#include "PhysicsEngine.h"
#include "../BaseObject.h"

PhysicsActor::PhysicsActor(PhysicsEngine* physicsEngine, BaseObject* owner)
	: physicsEngine_(physicsEngine), owner_(owner)
{
	physicsEngine_->RegisterActor(this);
	transform_ = owner_->GetTransform();
}

PhysicsActor::~PhysicsActor()
{
    physicsEngine_->UnregisterActor(this);
}

void PhysicsActor::ResetFlags()
{
	collidedTop_ = false;
	collidedBottom_ = false;
	collidedLeft_ = false;
	collidedRight_ = false;
}
