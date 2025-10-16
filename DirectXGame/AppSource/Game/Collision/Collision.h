#pragma once
#include "Math/MyMath.h"

class Collider;

namespace Collision
{
	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="a">コライダーA</param>
	/// <param name="b">コライダーB</param>
	/// <returns>判定結果</returns>
	bool CheckCollision(const Collider& a, const Collider& b);

	/// <summary>
	/// 円の当たり判定
	/// </summary>
	/// <param name="a">円a</param>
	/// <param name="b">円b</param>
	/// <returns>判定結果</returns>
	bool CheckSphere(const Sphere& a, const Sphere& b);


	/// <summary>
	/// AABBの当たり判定
	/// </summary>
	/// <param name="a">aのAABB</param>
	/// <param name="b">bのAABB</param>
	/// <returns>判定結果</returns>
	bool CheckAABB(const AABB& a, const AABB& b);


	/// <summary>
	/// 円の中心がAABB内に含まれるか判定
	/// </summary>
	/// <param name="point">点</param>
	/// <param name="center">円の中心</param>
	/// <param name="radius">半径</param>
	/// <returns></returns>
	bool CheckPointInSphere(const Vector3& point, const Sphere& sphere);
}

