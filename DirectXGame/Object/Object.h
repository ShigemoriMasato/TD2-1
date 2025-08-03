#pragma once
#include "../Engine/Data/Transform.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Render/MyDirectX.h"
#include <string>
#include <memory>

enum class ShapeType {
	Triangle,
	Sphere,
	Cube,
	Sprite,
	Line,
	ThickLine,
	Model,
	Count
};

enum class CollisionType {
	Sphere,
	Cupsule,

	CollisionTypeCount
};

class Object {
public:

	Object(Camera* camera, ShapeType type) : camera_(camera), type_(type), transform_(std::make_unique<Transform>()) {};
	virtual ~Object() = default;
	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw(const Matrix4x4* worldMatrix = nullptr) const;

	virtual void OnCollision(Object* other) {}

	/// <summary>
	/// 三角形、スプライト、球のときのみ適用
	/// </summary>
	/// <param name="lt">三角形左下、スプライト左上、球の半径</param>
	/// <param name="rt">三角形上、スプライト右上</param>
	/// <param name="lb">三角形右下、スプライト左下</param>
	/// <param name="rb">スプライト右下</param>
	void SetLocalPosition(const Vector3& lt = UniqueNumber::Vec3::min, const Vector3& rt = UniqueNumber::Vec3::min, const Vector3& lb = UniqueNumber::Vec3::min, const Vector3& rb = UniqueNumber::Vec3::min);
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetShapeType(ShapeType type) { type_ = type; }

	virtual Transform GetTransform() const { return *transform_; }
	bool GetIsActive() const { return isActive_; }
	ShapeType GetShapeType() const { return type_; }
	Vector3 GetPrePosition() const { return prePos_; }

	void AddVelocity(const Vector3& velocity) { velocity_ += velocity; }

	std::string tag;
	CollisionType collisionType_ = CollisionType::Sphere; //当たり判定の種類

protected:

	Vector3 velocity_ = {};								//Actor用
	std::shared_ptr<Transform> transform_{};
	Vector3 prePos_ = {};								//使う場合、各自Updateなどで自分で更新すること
	uint32_t color = 0xffffffff;
	int handle_ = WHIETE1x1;

	bool isActive_ = true;

	Camera* camera_ = nullptr;

	//当たり判定用
	Vector3 size_ = { 1.0f, 1.0f, 1.0f };

private:

	ShapeType type_ = ShapeType::Count;

	Vector3 lt = { -0.5f, 0.5f, 0.0f };
	Vector3 rt = { 0.5f, 0.5f, 0.0f };
	Vector3 lb = { -0.5f, -0.5f, 0.0f };
	Vector3 rb = { 0.5f, -0.5f, 0.0f };
};

