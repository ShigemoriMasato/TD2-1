#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector3.h"
#include <cassert>
#include <cmath>

float cot(float radian) {
	return 1.0f / std::tanf(radian);
}

Matrix4x4 MakeIdentity4x4() {
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const {
	Matrix4x4 ans;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ans.m[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				ans.m[i][j] += m[i][k] * mat.m[k][j];
			}
		}
	}
	return ans;
}

Vector3 Matrix4x4::operator*(const Vector3& vec) const {
	Vector3 ans;
	ans.x = m[0][0] * vec.x + m[1][0] * vec.y + m[2][0] * vec.z + m[3][0];
	ans.y = m[0][1] * vec.x + m[1][1] * vec.y + m[2][1] * vec.z + m[3][1];
	ans.z = m[0][2] * vec.x + m[1][2] * vec.y + m[2][2] * vec.z + m[3][2];
	float w = m[0][3] * vec.x + m[1][3] * vec.y + m[2][3] * vec.z + m[3][3];
	assert(fabsf(w) > 1e-6);
	ans.x /= w;
	ans.y /= w;
	ans.z /= w;
	return ans;
}

Vector4 Matrix4x4::operator*(const Vector4& vec) const {
	Vector4 ans;
	ans.x = m[0][0] * vec.x + m[1][0] * vec.y + m[2][0] * vec.z + m[3][0] * vec.w;
	ans.y = m[0][1] * vec.x + m[1][1] * vec.y + m[2][1] * vec.z + m[3][1] * vec.w;
	ans.z = m[0][2] * vec.x + m[1][2] * vec.y + m[2][2] * vec.z + m[3][2] * vec.w;
	ans.w = m[0][3] * vec.x + m[1][3] * vec.y + m[2][3] * vec.z + m[3][3] * vec.w;
	return ans;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	return {
		cot(fovY / 2) / aspectRatio, 0, 0, 0,
		0, cot(fovY / 2), 0, 0,
		0, 0, farClip / (farClip - nearClip), 1,
		0, 0, (-nearClip * farClip) / (farClip - nearClip), 0
	};
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	return {
		2 / (right - left), 0, 0, 0,
		0, 2 / (top - bottom), 0, 0,
		0, 0, 1 / (farClip - nearClip), 0,
		(left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip), 1
	};
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	return {
		width / 2, 0, 0, 0,
		0, -height / 2, 0, 0,
		0, 0, maxDepth - minDepth, 0,
		left + width / 2, top + height / 2, minDepth, 1
	};
}

Matrix4x4 Inverse(const Matrix4x4& a) {

	float d = a.m[0][0] * a.m[1][1] * a.m[2][2] * a.m[3][3]
		+ a.m[0][0] * a.m[1][2] * a.m[2][3] * a.m[3][1]
		+ a.m[0][0] * a.m[1][3] * a.m[2][1] * a.m[3][2]
		- a.m[0][0] * a.m[1][3] * a.m[2][2] * a.m[3][1]
		- a.m[0][0] * a.m[1][2] * a.m[2][1] * a.m[3][3]
		- a.m[0][0] * a.m[1][1] * a.m[2][3] * a.m[3][2]
		- a.m[0][1] * a.m[1][0] * a.m[2][2] * a.m[3][3]
		- a.m[0][2] * a.m[1][0] * a.m[2][3] * a.m[3][1]
		- a.m[0][3] * a.m[1][0] * a.m[2][1] * a.m[3][2]
		+ a.m[0][3] * a.m[1][0] * a.m[2][2] * a.m[3][1]
		+ a.m[0][2] * a.m[1][0] * a.m[2][1] * a.m[3][3]
		+ a.m[0][1] * a.m[1][0] * a.m[2][3] * a.m[3][2]
		+ a.m[0][1] * a.m[1][2] * a.m[2][0] * a.m[3][3]
		+ a.m[0][2] * a.m[1][3] * a.m[2][0] * a.m[3][1]
		+ a.m[0][3] * a.m[1][1] * a.m[2][0] * a.m[3][2]
		- a.m[0][3] * a.m[1][2] * a.m[2][0] * a.m[3][1]
		- a.m[0][2] * a.m[1][1] * a.m[2][0] * a.m[3][3]
		- a.m[0][1] * a.m[1][3] * a.m[2][0] * a.m[3][2]
		- a.m[0][1] * a.m[1][2] * a.m[2][3] * a.m[3][0]
		- a.m[0][2] * a.m[1][3] * a.m[2][1] * a.m[3][0]
		- a.m[0][3] * a.m[1][1] * a.m[2][2] * a.m[3][0]
		+ a.m[0][3] * a.m[1][2] * a.m[2][1] * a.m[3][0]
		+ a.m[0][2] * a.m[1][1] * a.m[2][3] * a.m[3][0]
		+ a.m[0][1] * a.m[1][3] * a.m[2][2] * a.m[3][0];

	assert(fabsf(d) > 1e-6f);

	return {
		(a.m[1][1] * a.m[2][2] * a.m[3][3] + a.m[1][2] * a.m[2][3] * a.m[3][1] + a.m[1][3] * a.m[2][1] * a.m[3][2]
			- a.m[1][3] * a.m[2][2] * a.m[3][1] - a.m[1][2] * a.m[2][1] * a.m[3][3] - a.m[1][1] * a.m[2][3] * a.m[3][2]) / d,
		-(a.m[0][1] * a.m[2][2] * a.m[3][3] + a.m[0][2] * a.m[2][3] * a.m[3][1] + a.m[0][3] * a.m[2][1] * a.m[3][2]
			- a.m[0][3] * a.m[2][2] * a.m[3][1] - a.m[0][2] * a.m[2][1] * a.m[3][3] - a.m[0][1] * a.m[2][3] * a.m[3][2]) / d,
		(a.m[0][1] * a.m[1][2] * a.m[3][3] + a.m[0][2] * a.m[1][3] * a.m[3][1] + a.m[0][3] * a.m[1][1] * a.m[3][2]
			- a.m[0][3] * a.m[1][2] * a.m[3][1] - a.m[0][2] * a.m[1][1] * a.m[3][3] - a.m[0][1] * a.m[1][3] * a.m[3][2]) / d,
		-(a.m[0][1] * a.m[1][2] * a.m[2][3] + a.m[0][2] * a.m[1][3] * a.m[2][1] + a.m[0][3] * a.m[1][1] * a.m[2][2]
			- a.m[0][3] * a.m[1][2] * a.m[2][1] - a.m[0][2] * a.m[1][1] * a.m[2][3] - a.m[0][1] * a.m[1][3] * a.m[2][2]) / d,

		-(a.m[1][0] * a.m[2][2] * a.m[3][3] + a.m[1][2] * a.m[2][3] * a.m[3][0] + a.m[1][3] * a.m[2][0] * a.m[3][2]
			- a.m[1][3] * a.m[2][2] * a.m[3][0] - a.m[1][2] * a.m[2][0] * a.m[3][3] - a.m[1][0] * a.m[2][3] * a.m[3][2]) / d,
		(a.m[0][0] * a.m[2][2] * a.m[3][3] + a.m[0][2] * a.m[2][3] * a.m[3][0] + a.m[0][3] * a.m[2][0] * a.m[3][2]
			- a.m[0][3] * a.m[2][2] * a.m[3][0] - a.m[0][2] * a.m[2][0] * a.m[3][3] - a.m[0][0] * a.m[2][3] * a.m[3][2]) / d,
		-(a.m[0][0] * a.m[1][2] * a.m[3][3] + a.m[0][2] * a.m[1][3] * a.m[3][0] + a.m[0][3] * a.m[1][0] * a.m[3][2]
			- a.m[0][3] * a.m[1][2] * a.m[3][0] - a.m[0][2] * a.m[1][0] * a.m[3][3] - a.m[0][0] * a.m[1][3] * a.m[3][2]) / d,
		(a.m[0][0] * a.m[1][2] * a.m[2][3] + a.m[0][2] * a.m[1][3] * a.m[2][0] + a.m[0][3] * a.m[1][0] * a.m[2][2]
			- a.m[0][3] * a.m[1][2] * a.m[2][0] - a.m[0][2] * a.m[1][0] * a.m[2][3] - a.m[0][0] * a.m[1][3] * a.m[2][2]) / d,

		(a.m[1][0] * a.m[2][1] * a.m[3][3] + a.m[1][1] * a.m[2][3] * a.m[3][0] + a.m[1][3] * a.m[2][0] * a.m[3][1]
			- a.m[1][3] * a.m[2][1] * a.m[3][0] - a.m[1][1] * a.m[2][0] * a.m[3][3] - a.m[1][0] * a.m[2][3] * a.m[3][1]) / d,
		-(a.m[0][0] * a.m[2][1] * a.m[3][3] + a.m[0][1] * a.m[2][3] * a.m[3][0] + a.m[0][3] * a.m[2][0] * a.m[3][1]
			- a.m[0][3] * a.m[2][1] * a.m[3][0] - a.m[0][1] * a.m[2][0] * a.m[3][3] - a.m[0][0] * a.m[2][3] * a.m[3][1]) / d,
		(a.m[0][0] * a.m[1][1] * a.m[3][3] + a.m[0][1] * a.m[1][3] * a.m[3][0] + a.m[0][3] * a.m[1][0] * a.m[3][1]
			- a.m[0][3] * a.m[1][1] * a.m[3][0] - a.m[0][1] * a.m[1][0] * a.m[3][3] - a.m[0][0] * a.m[1][3] * a.m[3][1]) / d,
		-(a.m[0][0] * a.m[1][1] * a.m[2][3] + a.m[0][1] * a.m[1][3] * a.m[2][0] + a.m[0][3] * a.m[1][0] * a.m[2][1]
			- a.m[0][3] * a.m[1][1] * a.m[2][0] - a.m[0][1] * a.m[1][0] * a.m[2][3] - a.m[0][0] * a.m[1][3] * a.m[2][1]) / d,

		-(a.m[1][0] * a.m[2][1] * a.m[3][2] + a.m[1][1] * a.m[2][2] * a.m[3][0] + a.m[1][2] * a.m[2][0] * a.m[3][1]
			- a.m[1][2] * a.m[2][1] * a.m[3][0] - a.m[1][1] * a.m[2][0] * a.m[3][2] - a.m[1][0] * a.m[2][2] * a.m[3][1]) / d,
		(a.m[0][0] * a.m[2][1] * a.m[3][2] + a.m[0][1] * a.m[2][2] * a.m[3][0] + a.m[0][2] * a.m[2][0] * a.m[3][1]
			- a.m[0][2] * a.m[2][1] * a.m[3][0] - a.m[0][1] * a.m[2][0] * a.m[3][2] - a.m[0][0] * a.m[2][2] * a.m[3][1]) / d,
		-(a.m[0][0] * a.m[1][1] * a.m[3][2] + a.m[0][1] * a.m[1][2] * a.m[3][0] + a.m[0][2] * a.m[1][0] * a.m[3][1]
			- a.m[0][2] * a.m[1][1] * a.m[3][0] - a.m[0][1] * a.m[1][0] * a.m[3][2] - a.m[0][0] * a.m[1][2] * a.m[3][1]) / d,
		(a.m[0][0] * a.m[1][1] * a.m[2][2] + a.m[0][1] * a.m[1][2] * a.m[2][0] + a.m[0][2] * a.m[1][0] * a.m[2][1]
			- a.m[0][2] * a.m[1][1] * a.m[2][0] - a.m[0][1] * a.m[1][0] * a.m[2][2] - a.m[0][0] * a.m[1][2] * a.m[2][1]) / d
	};
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
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
