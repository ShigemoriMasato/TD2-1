#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Data.h"
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
	int32_t enableLighting = true;
	float padding[3];
	Matrix4x4 uvTransform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f ,0.0f, 1.0f, 0.0f
	};
};

struct ModelMaterial final {
	std::string name;
	int textureHandle;
};

struct TramsformMatrixData final {
	Matrix4x4 wvp;
	Matrix4x4 world;
};

struct DirectionalLightData final {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct ModelData final {
	std::unordered_map<std::string, std::vector<VertexData>> vertices;
	std::vector<ModelMaterial> material{};
};

struct AABB final {
	Vector3 min{ 0.0f, 0.0f, 0.0f };
	Vector3 max{ 0.0f, 0.0f, 0.0f };
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct DistanceSample {
	float t;       // 曲線上の割合
	float distance; // 始点からの累積距離
};

struct Capsule {
	Vector3 start;
	Vector3 end;
	float radius;
};
