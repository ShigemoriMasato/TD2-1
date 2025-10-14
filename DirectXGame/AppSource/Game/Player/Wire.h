#pragma once
#include "../BaseObject.h"
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

#include <memory>

class Wire : public BaseObject
{
public:

	void Initialize(ModelData* modelData, Camera* camera)override;

	void Update(float deltaTime)override;

	std::pair<Transform, float>GetCollider()const { return { transform_,radius_ }; }
private:


};

