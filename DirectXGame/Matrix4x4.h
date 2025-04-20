#pragma once

struct Vector4;
struct Vector3;

struct Matrix4x4 final {
	float m[4][4];

	Matrix4x4 operator* (const Matrix4x4& mat) const;
	Vector3 operator* (const Vector3& vec) const;
	Vector4 operator* (const Vector4& vec) const;
};

float cot(float radian);

Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeRotateMatrix(const Vector3& rotation);
Matrix4x4 MakeRotetaXMatrix(float radius);
Matrix4x4 MakeRotateYMatrix(float radius);
Matrix4x4 MakeRotateZMatrix(float radius);

Matrix4x4 Inverse(const Matrix4x4& a);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
