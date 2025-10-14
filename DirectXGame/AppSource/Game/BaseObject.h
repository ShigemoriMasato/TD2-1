#pragma once
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

#include <memory>

class AudioManager; 
class BaseObject
{
public:
	virtual ~BaseObject() = default;

	virtual void Initialize(ModelData* modelData, Camera* camera);

	virtual void Update(float deltaTime) = 0;

	virtual void Draw(Render* render);

	//配列から消すかどうかのフラグ
	virtual bool Finalize() { return false; }

	virtual void OnCollision(BaseObject* other){}

	std::pair<Transform, float>GetCollider()const { return { transform_,radius_ }; }

protected://オブジェクトプロパティ
	Transform transform_;
	float radius_ = 0.0f;

protected://描画に必要なリソース
	std::unique_ptr<ModelResource> modelResource_;

};

