#pragma once
#include "Object/Object.h"
#include "Object/Collision.h"
#include "CometConfig.h"

class CometCollision : public Object {
public:
	CometCollision(Camera* camera, bool isSub = false);
	~CometCollision() = default;

	void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

	void SetPosition(const Vector3& position) { transform_->position = position; if (nearCollision_) nearCollision_->SetPosition(position); };

private:

	std::shared_ptr<CometCollision> nearCollision_;

};

class Comet : public Object {
public:

	Comet(Camera* camera, std::array<int, 3> modelHandle, Vector3* playerTransformPtr);
	~Comet() = default;

	void Initialize();

	void Update();

	void SetPosition(const Vector3& position);
	void AddMovement(const Vector3& movement);

	void OnCollision(Object* other) override;

	void SetConfig(const CometConfig& config);
	CometConfig GetConfig() const;

	Vector3* GetPositionPtr() const;

	void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

private:

	std::array<int, 3> modelHandle_;

	Vector3* playerPosition_;

	std::shared_ptr<CometCollision> collisionRender_;

	std::shared_ptr<RenderCollision> mainCollision_;
	std::shared_ptr<RenderCollision> nearCollision_;
};

