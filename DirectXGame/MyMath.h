#pragma once
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
};

struct VertexMatrix final {
	float m[4][4];
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
