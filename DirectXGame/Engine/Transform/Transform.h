#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

class Camera;

struct Transform
{
	Vector3 scale{ 1.0f, 1.0f, 1.0f }; //拡大縮小
	Vector3 rotation{ 0.0f, 0.0f, 0.0f }; //回転
	Vector3 position{ 0.0f, 0.0f, 0.0f }; //位置
};


struct VertexData final
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct ModelVertexData final
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	int nodeIndex;
};

struct Material final
{
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Matrix4x4 uvTransform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f ,0.0f, 1.0f, 0.0f
	};
	float shininess = 0.0f;
};

struct ParticleData final
{
	Matrix4x4 wvp;
	Matrix4x4 world;
	Vector4 color;
};

struct MatrixData final
{
	Matrix4x4 wvp;
	Matrix4x4 world;
};

struct DirectionalLightData final
{
	int32_t enableLighting = 0;
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector3 direction = { -0.68f, 0.72f, -0.12f };
	float intensity = 1.0f;
};

struct AABB final
{
	Vector3 min{ 0.0f, 0.0f, 0.0f };
	Vector3 max{ 0.0f, 0.0f, 0.0f };

	Vector3 Position()const
	{
		Vector3 pos;
		pos.x = (min.x + max.x) * 0.5f;
		pos.y = (min.y + max.y) * 0.5f;
		pos.z = (min.z + max.z) * 0.5f;
		return pos;
	}
	Vector3 Size()const
	{
		Vector3 size;
        size.x = max.x - min.x;
        size.y = max.y - min.y;
        size.z = max.z - min.z;
		return size;
	}
};

struct Sphere
{
	Vector3 center;
	float radius;
};