#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

struct Transform final {
	Vector3 scale;
	Vector3 rotation;
	Vector3 position;
};;

Matrix4x4 MakeTransformMatrix(const Transform& transform);

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Matrix4x4 MakeRotateMatrix(const Vector3& rotation);

Matrix4x4 MakeRotetaXMatrix(float radius);

Matrix4x4 MakeRotateYMatrix(float radius);

Matrix4x4 MakeRotateZMatrix(float radius);
