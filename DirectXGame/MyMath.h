#pragma once
#include <cstdint>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

struct Transform final {
	Vector3 scale;
	Vector3 rotation;
	Vector3 position;
};

struct VertexData final {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData final {
	Vector4 color;
	int32_t enableLighting;
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

Matrix4x4 MakeTransformMatrix(const Transform& transform);

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Matrix4x4 MakeRotateMatrix(const Vector3& rotation);

Matrix4x4 MakeRotetaXMatrix(float radius);

Matrix4x4 MakeRotateYMatrix(float radius);

Matrix4x4 MakeRotateZMatrix(float radius);

Vector3 TransForm(const Vector3& vector, const Matrix4x4& matrix);

void ConvertVector(const Vector3& vec, Vector4& outVec);

void ConvertVector(const Vector4& vec, Vector3& outVec);

void Normalize(Vector3& vec);
