#include "PhysicsActor.h"
#include "PhysicsEngine.h"
#include "../BaseObject.h"

std::unique_ptr<Logger> PhysicsActor::logger_ = nullptr;

PhysicsActor::PhysicsActor(PhysicsEngine* physicsEngine, BaseObject* owner)
	: physicsEngine_(physicsEngine), owner_(owner)
{
	if (!logger_) {
		logger_ = std::make_unique<Logger>();
		logger_->RegistLogFile("BaseClass");
	}

	myClassName_ = typeid(owner).name();
	logger_->Log("Create:	[PhysicsActor]	[" + myClassName_ + "]");

	physicsEngine_->RegisterActor(this);
	transform_ = owner_->GetTransform();
}

PhysicsActor::~PhysicsActor()
{
	logger_->Log("Delete:	[PhysicsActor]	[" + myClassName_ + "]");

    physicsEngine_->UnregisterActor(this);
}

void PhysicsActor::ResetFlags()
{
	collidedTop_ = false;
	collidedBottom_ = false;
	collidedLeft_ = false;
	collidedRight_ = false;
}
