#include "BaseObject.h"


void BaseObject::Initialize(ModelData* modelData, Camera* camera)
{
	assert(modelData);
    assert(camera);

	modelResource_ = std::make_unique<ModelResource>();
	modelResource_->Initialize(modelData);
	modelResource_->SetCamera(camera);
	modelResource_->psoConfig_.isSwapChain = true;
}

void BaseObject::Draw(Render* render)
{
	modelResource_->SetMatrixData(transform_.scale, transform_.rotation, transform_.position);
	render->Draw(modelResource_.get());


}
