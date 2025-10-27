#pragma once
#include "Collision/Collider.h"
#include "Physics/PhysicsActor.h"
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

#include <memory>

class AudioManager; 
class BaseObject
{
public:
	BaseObject();
	virtual ~BaseObject();

	static void SetAudioManager(AudioManager* audioManager) {
		audio_ = audioManager;
	}

	virtual void Initialize(ModelData* modelData, Camera* camera);

	virtual void Update(float deltaTime) = 0;

	virtual void Draw(Render* render);

	//配列から消すかどうかのフラグ
	bool IsDead() const { return !isAlive_; }

	virtual void OnCollision(BaseObject* other){}

	Collider* GetCollider()const { return collider_.get(); }
	PhysicsActor* GetActor() { return actor_.get(); }
	Transform* GetTransform() { return &transform_; }
	void SetActor(PhysicsEngine* physicsEngine,bool useGravity = true);
	void SetPosition(const Vector3& position) { transform_.position = position; }
	void SetColor(uint32_t color) { modelResource_->color_ = color; }
protected://オブジェクトプロパティ
	Transform transform_;
	std::unique_ptr<PhysicsActor> actor_ = nullptr;
	std::unique_ptr<Collider> collider_;
	bool isAlive_ = true;
protected://描画に必要なリソース
	std::unique_ptr<ModelResource> modelResource_;

protected://カスの引数
	static AudioManager* audio_;

public://debug用
	static std::unique_ptr<Logger> logger_;
	std::string myClassName_;
};

