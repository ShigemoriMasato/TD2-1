#include "Collision.h"
#include "Collider.h"
#include <algorithm>

using namespace MyMath;
namespace Collision
{

	bool CheckCollision(const Collider& a, const Collider& b)
	{
		const Transform& aTransform = a.GetTransform();
		const Transform& bTransform = b.GetTransform();

		const Vector3& aSize = aTransform.scale * a.GetSize();
		const Vector3& bSize = bTransform.scale * b.GetSize();
		const Vector3& aCenter = aTransform.position;
		const Vector3& bCenter = bTransform.position;

		Vector3 aHalfSize = aSize * 0.5f;
		Vector3 bHalfSize = bSize * 0.5f;

		AABB aAABB;
		aAABB.max = aCenter + aHalfSize;
		aAABB.min = aCenter - aHalfSize;

		AABB bAABB;
		bAABB.max = bCenter + bHalfSize;
		bAABB.min = bCenter - bHalfSize;

		if (!CheckAABB(aAABB, bAABB))
			return false;

		float aRadius = std::max(aHalfSize.x, std::max(aHalfSize.y, aHalfSize.z));
		float bRadius = std::max(bHalfSize.x, std::max(bHalfSize.y, bHalfSize.z));

		Sphere aSphere = { aCenter,aRadius };
		Sphere bSphere = { bCenter,bRadius };
		if (a.GetType() == ColliderType::AABB && b.GetType() == ColliderType::AABB)
			return true;

		else if (a.GetType() == ColliderType::Sphere && b.GetType() == ColliderType::Sphere)
		{
			return CheckSphere(aSphere, bSphere);
		}
		else if (a.GetType() == ColliderType::AABB && b.GetType() == ColliderType::Sphere)
		{
			Vector3 nearestPoint;
			nearestPoint.x = std::clamp(bSphere.center.x, aAABB.min.x, aAABB.max.x);
			nearestPoint.y = std::clamp(bSphere.center.y, aAABB.min.y, aAABB.max.y);
			nearestPoint.z = std::clamp(bSphere.center.z, aAABB.min.z, aAABB.max.z);
			return CheckPointInSphere(nearestPoint, bSphere);
		}
		else if (a.GetType() == ColliderType::Sphere && b.GetType() == ColliderType::AABB)
		{
			Vector3 nearestPoint;
			nearestPoint.x = std::clamp(aSphere.center.x, bAABB.min.x, bAABB.max.x);
			nearestPoint.y = std::clamp(aSphere.center.y, bAABB.min.y, bAABB.max.y);
			nearestPoint.z = std::clamp(aSphere.center.z, bAABB.min.z, bAABB.max.z);
			return CheckPointInSphere(nearestPoint, aSphere);
		}
		return false;
	}

	bool CheckSphere(const Sphere& a, const Sphere& b)
	{

		return Length(a.center - b.center) <= a.radius + b.radius;
	}

	bool CheckAABB(const AABB& a, const AABB& b)
	{

		return  (a.max.x >= b.min.x && a.min.x <= b.max.x &&
				 a.max.y >= b.min.y && a.min.y <= b.max.y &&
				 a.max.z >= b.min.z && a.min.z <= b.max.z);
	}

	bool CheckPointInSphere(const Vector3& point, const Sphere& sphere)
	{
		return Length(point - sphere.center) <= sphere.radius;
	}

}