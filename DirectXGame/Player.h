#pragma once
#include "Object/Object.h"
#include "Object/Collision.h"
#include "PlayerBullet.h"
#include "Scene/Common/CommonData.h"
#include <vector>
#include <memory>
#include <list>

class Player : public Object {
public:

	Player(Camera* camera, Camera* parent, CommonData* cd, float* speed);
	~Player() = default;

	void Initialize();

	void Update();

	/// <summary>
	/// BulletとPlayerのDraw
	/// </summary>
	void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

	std::vector<std::shared_ptr<PlayerBullet>> GetBullets() { return bullets_; }
	Vector3 GetDirection() const { return direction_; }

	void SetBulletTargetPosition(const Vector3* position);

	void OnCollision(Object* other) override;

	Vector3* GetPositionPtr() { return &transform_->position; }

	Collision* GetCollision() { return collision_.get(); }

private:

	Vector3 velocity_{};
	Vector3 direction_{};
	float* speed_ = nullptr;

	Camera* parentCamera_ = nullptr;

	std::shared_ptr<RenderCollision> collision_;

	int cooltime_ = 0;
	const int maxCooltime_ = 5;
	int bulletModelHandle_ = 0;
	std::vector<std::shared_ptr<PlayerBullet>> bullets_;

	int reticleHandle_ = 0;
	Transform reticleTransform_{};
	Matrix4x4 reticleWorldMatrix_{};
	float reticleMoveAdjustment_ = 1.25f;
	float reticleSpeed_ = 0.2f;
	int reticleIndex_ = 0;
	bool isXBoxController_ = false;

	bool isStalking_ = true;

	bool hited_ = false;
	bool hit_ = false;

	Transform fpsCameraTransform_;

	std::shared_ptr<Transform> playerTransform_ = nullptr;

	Matrix4x4 screenTransform_{};

	std::list<Vector3*> bulletTargetPositions_;
};
