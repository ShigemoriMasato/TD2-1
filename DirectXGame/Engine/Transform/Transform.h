#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

class Camera;


struct VertexData final {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct ModelVertexData final {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	int nodeIndex;
};

struct Material final {
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Matrix4x4 uvTransform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f ,0.0f, 1.0f, 0.0f
	};
	float shininess = 0.0f;
};

struct ParticleData final {
	Matrix4x4 wvp;
	Matrix4x4 world;
	Vector4 color;
};

struct MatrixData final {
	Matrix4x4 wvp;
	Matrix4x4 world;
};

struct DirectionalLightData final {
	int32_t enableLighting = 0;
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector3 direction = { -0.68f, 0.72f, -0.12f };
	float intensity = 1.0f;
};

struct AABB final {
	Vector3 min{ 0.0f, 0.0f, 0.0f };
	Vector3 max{ 0.0f, 0.0f, 0.0f };
};

struct Sphere {
	Vector3 center;
	float radius;
};

//即興クオータニオン。仕組みはよくわかってない。AI制なので、いい感じに変えて
struct Quaternion {
	float w = 1.0f, x = 0.0f, y = 0.0f, z = 0.0f;

	// 単位クオータニオン生成（回転なし）
	static Quaternion Identity() {
		return { 1, 0, 0, 0 };
	}

	// 軸と角度から生成
	static Quaternion FromAxisAngle(const Vector3& axis, float angle) {
		float half = angle * 0.5f;
		float s = sin(half);
		return {
			cos(half),
			axis.x * s,
			axis.y * s,
			axis.z * s
		};
	}

	// 正規化
	Quaternion Normalize() const {
		float len = sqrt(w * w + x * x + y * y + z * z);
		if (len == 0.0f) {
			return { 1, 0, 0, 0 };
		}
		return { w / len, x / len, y / len, z / len };
	}

	// 掛け算（回転合成）
	Quaternion operator*(const Quaternion& q) const {
		return {
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w
		};
	}

	Matrix4x4 ToMatrix() const {
		// 正規化
		Quaternion a = Normalize();

		return {
			1 - 2 * (a.y * a.y + a.z * a.z),  2 * (a.x * a.y + a.z * a.w),    2 * (a.x * a.z - a.y * a.w),    0,
			2 * (a.x * a.y - a.z * a.w),      1 - 2 * (a.x * a.x + a.z * a.z),2 * (a.y * a.z + a.x * a.w),    0,
			2 * (a.x * a.z + a.y * a.w),      2 * (a.y * a.z - a.x * a.w),    1 - 2 * (a.x * a.x + a.y * a.y),0,
			0,                             0,                          0,                          1
		};
	}
};

struct EulerTransform {
	Vector3 scale{ 1.0f, 1.0f, 1.0f }; //拡大縮小
	Vector3 rotation{ 0.0f, 0.0f, 0.0f }; //回転
	Vector3 position{ 0.0f, 0.0f, 0.0f }; //位置
};

struct QuaternionTransform {
	Vector3 scale{ 1.0f,1.0f,1.0f };
	Quaternion rotation{};
	Vector3 position{};
};
