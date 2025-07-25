#pragma once
#include "Object/Object.h"
#include "PlayerBullet.h"
#include <vector>
#include <memory>

class Player : public Object {
public:

	Player(Camera* camera, Camera* parent, int modelhandle, int bullethandle);
	~Player() = default;

	void Initialize() override;

	void Update() override;

	/// <summary>
	/// BulletとPlayerのDraw
	/// </summary>
	void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

	std::vector<std::shared_ptr<PlayerBullet>> GetBullets() { return bullets_; }

private:

	Vector3 velocity_{};
	const float speed_ = 0.01f;

	Camera* parentCamera_ = nullptr;

	int cooltime_ = 0;
	const int maxCooltime_ = 5;
	int bulletModelHandle_ = 0;
	std::vector<std::shared_ptr<PlayerBullet>> bullets_;

	std::shared_ptr<Transform> playerTransform_ = nullptr;

	Matrix4x4 screenTransform_{};
};
