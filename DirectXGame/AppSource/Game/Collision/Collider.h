#pragma once
#include "Collision.h"

enum class ColliderType
{
	None,
	AABB,
	Sphere,
};

enum class ColliderTag
{
	Dynamic = 0,	//動くオブジェクト
	Static			//静的なオブジェクト
};

class Collider
{
public:
	virtual ~Collider() = default;
	virtual ColliderType GetType() const = 0;

	//ワールド座標系での最小AABBを取得
	AABB GetWorldAABB()const;

	//セッター/ゲッター
	void SetSize(const Vector3& size) { size_ = size; }
	const Vector3& GetSize() const { return size_; }

	const Transform& GetTransform() const { return *transform_; }
	void SetTransform(Transform* transform) { transform_ = transform; }

	ColliderTag GetTag() const { return tag_; }
	void SetTag(ColliderTag tag) { tag_ = tag; }

	bool IsActive() const { return isActive_; }
	bool IsTrigger() const { return isTrigger_; }

	void SetActive(bool isActive) { isActive_ = isActive; }
	void SetTrigger(bool isTrigger) { isTrigger_ = isTrigger; }
protected:
	Vector3 size_ = { 0.0f,0.0f,0.0f };		// 当たり判定の最小サイズ

	Transform* transform_ = nullptr;		// 所属のオブジェクト
	ColliderTag tag_;						// タグ

	bool isTrigger_ = false;				// トリガーであるか
	bool isActive_ = true;					// 有効性
};

class AABBCollider : public Collider
{
public:
	explicit AABBCollider(const Vector3& size)
	{
		SetSize(size);
		aabb_.max = size_ * 0.5f;
		aabb_.min = -aabb_.max;
	}

	ColliderType GetType() const override
	{
		return ColliderType::AABB;
	}

	const AABB& GetAABB() { return aabb_; }

private:
	AABB aabb_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
};

class SphereCollider : public Collider
{
public:
	explicit SphereCollider(float radius) :radius_(radius) { SetSize(Vector3(radius, radius, radius)); }

	ColliderType GetType() const override
	{
		return ColliderType::Sphere;
	}

	float GetRadius()const { return radius_; }

private:
	float radius_ = 0.0f;
};


inline AABB Collider::GetWorldAABB() const
{
	if (!transform_)
		return AABB();

	Vector3 position = transform_->position;
	Vector3 scaledSize = GetSize() * transform_->scale;

	return AABB(position - scaledSize, position + scaledSize);
}
