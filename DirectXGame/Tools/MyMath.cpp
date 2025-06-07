#include "MyMath.h"
#include <cmath>
#include <cassert>

Matrix4x4 operator*(const Matrix4x4& mat1, const Matrix4x4& mat2) {
	Matrix4x4 ans = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				ans.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
			}
		}
	}
	return ans;
}

Vector3 operator*(const Matrix4x4& mat, const Vector3& vec) {
	Vector3 ans;
	ans.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
	ans.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
	ans.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];
	float w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + mat.m[3][3];
	assert(w != 0.0f);
	ans.x /= w;
	ans.y /= w;
	ans.z /= w;
	return ans;
}

Vector3 operator*(const Vector3& vec, const Matrix4x4& mat) {
	return mat * vec;
}

Matrix4x4 MakeTransformMatrix(const Transform& transform) {
	return MakeScaleMatrix(transform.scale) *
		MakeRotateMatrix(transform.rotation) *
		MakeTranslateMatrix(transform.position);
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		translate.x, translate.y, translate.z, 1
	};

}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	return {
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	};

}

Matrix4x4 MakeRotateMatrix(const Vector3& rotation) {
	Matrix4x4 matX = MakeRotetaXMatrix(rotation.x);
	Matrix4x4 matY = MakeRotateYMatrix(rotation.y);
	Matrix4x4 matZ = MakeRotateZMatrix(rotation.z);
	return matX * matY * matZ;
}

Matrix4x4 MakeRotetaXMatrix(float radius) {

	return {
		1, 0, 0, 0,
		0, std::cosf(radius), std::sinf(radius), 0,
		0, -std::sinf(radius), std::cosf(radius), 0,
		0, 0, 0, 1
	};

}

Matrix4x4 MakeRotateYMatrix(float radius) {

	return {
		std::cosf(radius), 0, -std::sinf(radius), 0,
		0, 1, 0, 0,
		std::sinf(radius), 0, std::cosf(radius), 0,
		0, 0, 0, 1
	};

}

Matrix4x4 MakeRotateZMatrix(float radius) {

	return { std::cosf(radius), std::sinf(radius), 0, 0,
		-std::sinf(radius), std::cosf(radius), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

}

Vector3 TransForm(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 ans;
	ans.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	ans.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	ans.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	ans.x /= w;
	ans.y /= w;
	ans.z /= w;
	return ans;
}

void ConvertVector(const Vector3& vec, Vector4& outVec) {
	outVec.x = vec.x;
	outVec.y = vec.y;
	outVec.z = vec.z;
	outVec.w = 1.0f;
}

void ConvertVector(const Vector4& vec, Vector3& outVec) {
	outVec.x = vec.x;
	outVec.y = vec.y;
	outVec.z = vec.z;
}

void Normalize(Vector3& vec) {
	float length = std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	if (length == 0.0f) {
		vec = {};
		return;
	}
	vec.x /= length;
	vec.y /= length;
	vec.z /= length;
}
