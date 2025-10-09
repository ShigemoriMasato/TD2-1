#pragma once
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

#include <memory>

class AudioManager;
class BaseObject
{
public:
	virtual ~BaseObject() = default;

	virtual void Initialize(ModelData* modelData,Camera* camera);
	
	virtual void Update() = 0;

	virtual void Draw(Render* render);

	virtual bool Finalize() { return false; }
	
protected:
	
	std::unique_ptr<ModelResource> modelResource_;
	Transform transform_;

};

