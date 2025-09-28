#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

class Camera;

struct Transform {
	Vector3 scale{ 1.0f, 1.0f, 1.0f }; //拡大縮小
	Vector3 rotation{ 0.0f, 0.0f, 0.0f }; //回転
	Vector3 position{ 0.0f, 0.0f, 0.0f }; //位置
};


struct VertexData final {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData final {
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	int32_t enableLighting = 0;
	float padding[3];
	Matrix4x4 uvTransform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f ,0.0f, 1.0f, 0.0f
	};
};

struct MatrixData final {
	Matrix4x4 wvp;
	Matrix4x4 world;
};

struct DirectionalLightData final {
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