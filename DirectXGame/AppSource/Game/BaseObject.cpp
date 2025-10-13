#include "BaseObject.h"


void BaseObject::Initialize(ModelData* modelData, Camera* camera)
{
	assert(modelData);
    assert(camera);

	modelResource_ = std::make_unique<ModelResource>();
	modelResource_->Initialize(modelData);
	modelResource_->camera_ = camera;
}

void BaseObject::Draw(Render* render)
{
	render->Draw(modelResource_.get());


}
