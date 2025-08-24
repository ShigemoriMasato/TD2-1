#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "UniqueNumber.h"
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
