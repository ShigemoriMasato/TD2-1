#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#include <cmath>
#include <cstdint>

Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, float b);
Vector2 operator*(float a, const Vector2& b);
Vector2 operator/(const Vector2& a, const Vector2& b);
Vector2 operator/(const Vector2& a, float b);
Vector2 operator+=(Vector2& a, const Vector2& b);
Vector2 operator-=(Vector2& a, const Vector2& b);
Vector2 operator*=(Vector2& a, const Vector2& b);
Vector2 operator*=(Vector2& a, float b);
Vector2 operator/=(Vector2& a, const Vector2& b);
Vector2 operator/=(Vector2& a, float b);
bool operator==(const Vector2& a, const Vector2& b);
bool operator!=(const Vector2& a, const Vector2& b);

Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, float b);
Vector3 operator*(float a, const Vector3& b);
Vector3 operator/(const Vector3& a, const Vector3& b);
Vector3 operator/(const Vector3& a, float b);
Vector3 operator+=(Vector3& a, const Vector3& b);
Vector3 operator-=(Vector3& a, const Vector3& b);
Vector3 operator*=(Vector3& a, const Vector3& b);
Vector3 operator*=(Vector3& a, float b);
Vector3 operator/=(Vector3& a, const Vector3& b);
Vector3 operator/=(Vector3& a, float b);
bool operator==(const Vector3& a, const Vector3& b);
bool operator!=(const Vector3& a, const Vector3& b);

Vector4 operator+(const Vector4& a, const Vector4& b);
Vector4 operator-(const Vector4& a, const Vector4& b);
Vector4 operator*(const Vector4& a, const Vector4& b);
Vector4 operator*(const Vector4& a, float b);
Vector4 operator*(float a, const Vector4& b);
Vector4 operator/(const Vector4& a, const Vector4& b);
Vector4 operator/(const Vector4& a, float b);
Vector4 operator+=(Vector4& a, const Vector4& b);
Vector4 operator-=(Vector4& a, const Vector4& b);
Vector4 operator*=(Vector4& a, const Vector4& b);
Vector4 operator*=(Vector4& a, float b);
Vector4 operator/=(Vector4& a, const Vector4& b);
Vector4 operator/=(Vector4& a, float b);
bool operator==(const Vector4& a, const Vector4& b);
bool operator!=(const Vector4& a, const Vector4& b);

Matrix3x3 operator+(const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator-(const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b);
Vector2 operator*(const Matrix3x3& a, const Vector2& b);
Vector2 operator*(const Vector2& a, const Matrix3x3& b);
Matrix3x3 operator/(const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator+=(Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator-=(Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator*=(Matrix3x3& a, const Matrix3x3& b);
bool operator==(const Matrix3x3& a, const Matrix3x3& b);
bool operator!=(const Matrix3x3& a, const Matrix3x3& b);

Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);
Vector3 operator*(const Matrix4x4& a, const Vector3& b);
Vector3 operator*(const Vector3& a, const Matrix4x4& b);
Matrix4x4 operator/(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator+=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator-=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator/=(Matrix4x4& a, const Matrix4x4& b);
bool operator==(const Matrix4x4& a, const Matrix4x4& b);
bool operator!=(const Matrix4x4& a, const Matrix4x4& b);

namespace TwoDimension {
	struct Translate {
		Vector2 pos;
		float angle; // in radians
		Vector2 scale;
	};;

	Matrix3x3 MakeInverseMatrix(const Matrix3x3& m);
	Matrix3x3 MakeIdentity();

	Matrix3x3 MakeTranslationMatrix(const Vector2& pos);
	Matrix3x3 MakeRotationMatrix(float angle);
	Matrix3x3 MakeScaleMatrix(const Vector2& scale);
	Matrix3x3 MakeAffineMatrix(const Vector2& pos, float angle, const Vector2& scale);
	Matrix3x3 MakeAffineMatrix(const Translate& t);
}

namespace ThreeDimension {
	struct Translate {
		Vector3 pos;
		Vector3 angle;
		Vector3 scale;
	};
	Matrix4x4 MakeInverseMatrix(const Matrix4x4& m);
	Matrix4x4 MakeIdentity();

	Matrix4x4 MakeTranslationMatrix(const Vector3& pos);
	Matrix4x4 MakeRotationMatrix(Vector3 angle);
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	Matrix4x4 MakeAffineMatrix(const Vector3& pos, const Vector3& angle, const Vector3& scale);
	Matrix4x4 MakeAffineMatrix(const Translate& t);

}

namespace MyMath {
	float lerp(float a, float b, float t);

	/// <summary>
	/// RGBAを線形補完
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	uint32_t lerp(uint32_t a, uint32_t b, float t);
}
