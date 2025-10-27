#include "BaseObject.h"
#include <typeinfo>

std::unique_ptr<Logger> BaseObject::logger_ = nullptr;
AudioManager* BaseObject::audio_ = nullptr;

BaseObject::BaseObject() {
}

BaseObject::~BaseObject() {
	logger_->Log("Delete:	[BaseObject]	[" + myClassName_ + "]");
}

void BaseObject::Initialize(ModelData* modelData, Camera* camera)
{
	assert(modelData);
    assert(camera);

	if (!logger_) {
		logger_ = std::make_unique<Logger>();
		logger_->RegistLogFile("BaseClass");
	}

	myClassName_ = typeid(*this).name();

	logger_->Log("Create:	[BaseObject]	[" + myClassName_ + "]");

	modelResource_ = std::make_unique<ModelResource>();
	modelResource_->Initialize(modelData);
	modelResource_->camera_ = camera;
}

void BaseObject::Draw(Render* render)
{
	render->Draw(modelResource_.get());


}

void BaseObject::SetActor(PhysicsEngine* physicsEngine, bool useGravity)
{
	actor_ = std::make_unique<PhysicsActor>(physicsEngine,this);
	actor_->useGravity_ = useGravity;
}
