#include "MyMath.h"
#include <cmath>
#include <cassert>

using namespace MyMath;
using namespace Matrix;

namespace {
	bool ZeroChecker(float* value, int size) {
		bool isNotZero = true;

		for (int i = 0; i < size; ++i) {
			if (std::fabsf(value[i]) <= 1e-6) {
				isNotZero = false;
				break;
			}
		}

		return isNotZero;
	}
}

Vector2 operator+(const Vector2& a, const Vector2& b) {
	return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
	return Vector2(a.x - b.x, a.y - b.y);
}

Vector2 operator*(const Vector2& a, const Vector2& b) {
	return Vector2(a.x * b.x, a.y * b.y);
}

Vector2 operator*(const Vector2& a, float b) {
	return Vector2(a.x * b, a.y * b);
}

Vector2 operator*(float a, const Vector2& b) {
	return Vector2(a * b.x, a * b.y);
}

Vector2 operator/(const Vector2& a, const Vector2& b) {
	if (!ZeroChecker((float*)&b, 2)) {
		assert(false && "Division by zero in Vector2 operator/");
	}
	return Vector2(a.x / b.x, a.y / b.y);
}

Vector2 operator/(const Vector2& a, float b) {
	if (std::fabsf(b) <= 1e-6) {
		assert(false && "Division by zero in Vector2 operator/");
	}
	return Vector2(a.x / b, a.y / b);
}

Vector2 operator+=(Vector2& a, const Vector2& b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

Vector2 operator-=(Vector2& a, const Vector2& b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

Vector2 operator*=(Vector2& a, const Vector2& b) {
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

Vector2 operator*=(Vector2& a, float b) {
	a.x *= b;
	a.y *= b;
	return a;
}

Vector2 operator/=(Vector2& a, const Vector2& b) {
	if (!ZeroChecker((float*)&b, 2)) {
		assert(false && "Division by zero in Vector2 operator/=");
	}
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

Vector2 operator/=(Vector2& a, float b) {
	if (std::fabsf(b) <= 1e-6) {
		assert(false && "Division by zero in Vector2 operator/=");
	}
	a.x /= b;
	a.y /= b;
	return a;
}

bool operator==(const Vector2& a, const Vector2& b) {
	return a.x == b.x && a.y == b.y;
}

bool operator!=(const Vector2& a, const Vector2& b) {
	return !(a == b);
}

Vector3 operator+(const Vector3& a, const Vector3& b) {
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator-(const Vector3& a, const Vector3& b) {
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator*(const Vector3& a, const Vector3& b) {
	return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 operator*(const Vector3& a, float b) {
	return Vector3(a.x * b, a.y * b, a.z * b);
}

Vector3 operator*(float a, const Vector3& b) {
	return Vector3(a * b.x, a * b.y, a * b.z);
}

Vector3 operator/(const Vector3& a, const Vector3& b) {
	if (!ZeroChecker((float*)&b, 3)) {
		assert(false && "Division by zero in Vector3 operator/");
	}
	return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vector3 operator/(const Vector3& a, float b) {
	if (!ZeroChecker(&b, 1)) {
		assert(false && "Division by zero in Vector3 operator/");
	}
	return Vector3(a.x / b, a.y / b, a.z / b);
}

Vector3 operator+=(Vector3& a, const Vector3& b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vector3 operator-=(Vector3& a, const Vector3& b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

Vector3 operator*=(Vector3& a, const Vector3& b) {
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}

Vector3 operator*=(Vector3& a, float b) {
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}

Vector3 operator/=(Vector3& a, const Vector3& b) {
	if (!ZeroChecker((float*)&b, 3)) {
		assert(false && "Division by zero in Vector3 operator/=");
	}
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}

Vector3 operator/=(Vector3& a, float b) {
	if (!ZeroChecker(&b, 1)) {
		assert(false && "Division by zero in Vector3 operator/=");
	}
	a.x /= b;
	a.y /= b;
	a.z /= b;
	return a;
}

bool operator==(const Vector3& a, const Vector3& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(const Vector3& a, const Vector3& b) {
	return !(a == b);
}

Vector4 operator+(const Vector4& a, const Vector4& b) {
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vector4 operator-(const Vector4& a, const Vector4& b) {
	return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

Vector4 operator*(const Vector4& a, const Vector4& b) {
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Vector4 operator*(const Vector4& a, float b) {
	return Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
}

Vector4 operator*(float a, const Vector4& b) {
	return Vector4(a * b.x, a * b.y, a * b.z, a * b.w);
}

Vector4 operator/(const Vector4& a, const Vector4& b) {
	if (!ZeroChecker((float*)&b, 4)) {
		assert(false && "Division by zero in Vector4 operator/");
	}
	return Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

Vector4 operator/(const Vector4& a, float b) {
	if (!ZeroChecker(&b, 1)) {
		assert(false && "Division by zero in Vector4 operator/");
	}
	return Vector4(a.x / b, a.y / b, a.z / b, a.w / b);
}

Vector4 operator+=(Vector4& a, const Vector4& b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

Vector4 operator-=(Vector4& a, const Vector4& b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

Vector4 operator*=(Vector4& a, const Vector4& b) {
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}

Vector4 operator*=(Vector4& a, float b) {
	a.x *= b;
	a.y *= b;
	a.z *= b;
	a.w *= b;
	return a;
}

Vector4 operator/=(Vector4& a, const Vector4& b) {
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}

Vector4 operator/=(Vector4& a, float b) {
	a.x /= b;
	a.y /= b;
	a.z /= b;
	a.w /= b;
	return a;
}

bool operator==(const Vector4& a, const Vector4& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

bool operator!=(const Vector4& a, const Vector4& b) {
	return !(a == b);
}

Matrix3x3 operator+(const Matrix3x3& a, const Matrix3x3& b) {
	Matrix3x3 result;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result.m[i][j] = a.m[i][j] + b.m[i][j];
		}
	}
	return result;
}

Matrix3x3 operator-(const Matrix3x3& a, const Matrix3x3& b) {
	Matrix3x3 result;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result.m[i][j] = a.m[j][i] - b.m[i][j];
		}
	}
	return result;
}

Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b) {
	Matrix3x3 result = {};
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				result.m[j][i] += a.m[k][i] * b.m[j][k];
			}
		}
	}
	return result;
}

Vector2 operator*(const Matrix3x3& m, const Vector2& v) {
	Vector2 ans;
	ans.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2];
	ans.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2];
	float w = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2];
	if (std::fabsf(w) <= 1e-6) {
		assert(false && "Division by zero in Vector2 operator* with Matrix3x3");
	}
	ans.x /= w;
	ans.y /= w;
	return ans;
}

Vector2 operator*(const Vector2& a, const Matrix3x3& b) {
	return b * a;
}

Matrix3x3 operator/(const Matrix3x3& a, const Matrix3x3& b) {
	if (ZeroChecker((float*)&b, 9)) {
		assert(false && "Division by zero in Matrix3x3 operator/");
	}
	return a * Inverse(b);
}

Matrix3x3 operator+=(Matrix3x3& a, const Matrix3x3& b) {
	a = a + b;
	return a;
}

Matrix3x3 operator-=(Matrix3x3& a, const Matrix3x3& b) {
	a = a - b;
	return a;
}

Matrix3x3 operator*=(Matrix3x3& a, const Matrix3x3& b) {
	a = a * b;
	return a;
}

Matrix3x3 operator/=(Matrix3x3& a, const Matrix3x3& b) {
	if (ZeroChecker((float*)&b, 9)) {
		assert(false && "Division by zero in Matrix3x3 operator/=");
	}
	a = a / b;
	return a;
}

bool operator==(const Matrix3x3& a, const Matrix3x3& b) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (a.m[i][j] != b.m[i][j]) {
				return false;
			}
		}
	}
	return true;
}

bool operator!=(const Matrix3x3& a, const Matrix3x3& b) {
	return !(a == b);
}

Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = a.m[i][j] + b.m[i][j];
		}
	}
	return result;
}

Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = a.m[i][j] - b.m[i][j];
		}
	}
	return result;
}

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += a.m[i][k] * b.m[k][j];
			}
		}
	}
	return result;
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

Vector3 operator*(const Vector3& a, const Matrix4x4& b) {
	return b * a;
}

Matrix4x4 operator/(const Matrix4x4& a, const Matrix4x4& b) {
	if (ZeroChecker((float*)&b, 16)) {
		assert(false && "Division by zero in Matrix4x4 operator/");
	}
	return a * Inverse(b);
}

Matrix4x4 operator+=(Matrix4x4& a, const Matrix4x4& b) {
	a = a + b;
	return a;
}

Matrix4x4 operator-=(Matrix4x4& a, const Matrix4x4& b) {
	a = a - b;
	return a;
}

Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b) {
	a = a * b;
	return a;
}

Matrix4x4 operator/=(Matrix4x4& a, const Matrix4x4& b) {
	a = a * Inverse(b);
	return a;
}

bool operator==(const Matrix4x4& a, const Matrix4x4& b) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (a.m[i][j] != b.m[i][j]) {
				return false;
			}
		}
	}
	return true;
}

bool operator!=(const Matrix4x4& a, const Matrix4x4& b) {
	return !(a == b);
}

float MyMath::lerp(float a, float b, float t) {
	return 0.0f;
}

uint32_t MyMath::lerp(uint32_t start, uint32_t end, float t) {
	int sa = start & 0xff;
	start >>= 8;
	int sb = start & 0xff;
	start >>= 8;
	int sg = start & 0xff;
	start >>= 8;
	int sr = start & 0xff;

	int ea = end & 0xff;
	end >>= 8;
	int eb = end & 0xff;
	end >>= 8;
	int eg = end & 0xff;
	end >>= 8;
	int er = end & 0xff;

	int aa = static_cast<int>(sa + (ea - sa) * t) << 24;
	int ab = static_cast<int>(sb + (eb - sb) * t) << 16;
	int ag = static_cast<int>(sg + (eg - sg) * t) << 8;
	int ar = static_cast<int>(sr + (er - sr) * t);

	return aa | ab | ag | ar;
}

Vector3 MyMath::ConvertVector(const Vector4& v) {
	return Vector3(v.x, v.y, v.z);
}

float MyMath::cot(float radian) {
	return std::cosf(radian) / std::sinf(radian);
}

Matrix3x3 Matrix::Inverse(const Matrix3x3& mat) {
	float d = mat.m[0][0] * mat.m[1][1] * mat.m[2][2] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0] + mat.m[0][2] * mat.m[1][0] * mat.m[2][1] -
		mat.m[0][2] * mat.m[1][1] * mat.m[2][0] - mat.m[0][1] * mat.m[1][0] * mat.m[2][2] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1];

	Matrix3x3 ans = { mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1] / d,
		-mat.m[0][1] * mat.m[2][2] + mat.m[0][2] * mat.m[2][1] / d,
		mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1] / d,

		-mat.m[1][0] * mat.m[2][2] + mat.m[1][2] * mat.m[2][0] / d,
		mat.m[0][0] * mat.m[2][2] - mat.m[0][2] * mat.m[2][0] / d,
		-mat.m[0][0] * mat.m[1][2] + mat.m[0][2] * mat.m[1][0] / d,

		mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0] / d,
		-mat.m[0][0] * mat.m[2][1] + mat.m[0][1] * mat.m[2][0] / d,
		mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0] / d
	};

	return mat;
}

Matrix3x3 Matrix::MakeIdentity3x3() {
	return Matrix3x3{ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
}

Matrix3x3 Matrix::MakeTranslationMatrix(const Vector2& translation) {
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		translation.x, translation.y, 1.0f
	};
}

Matrix3x3 Matrix::MakeRotationMatrix(float angle) {
	return {
		std::cosf(angle), std::sinf(angle), 0.0f,
		-sinf(angle), std::cosf(angle), 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix::MakeScaleMatrix(const Vector2& scale) {
	return {
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix::MakeAffineMatrix(const Vector2& translation, float angle, const Vector2& scale) {
	return MakeTranslationMatrix(translation) *
		MakeRotationMatrix(angle) *
		MakeScaleMatrix(scale);
}

Matrix4x4 Matrix::Inverse(const Matrix4x4& mat) {
	float d = mat.m[0][0] * mat.m[1][1] * mat.m[2][2] * mat.m[3][3]
		+ mat.m[0][0] * mat.m[1][2] * mat.m[2][3] * mat.m[3][1]
		+ mat.m[0][0] * mat.m[1][3] * mat.m[2][1] * mat.m[3][2]
		- mat.m[0][0] * mat.m[1][3] * mat.m[2][2] * mat.m[3][1]
		- mat.m[0][0] * mat.m[1][2] * mat.m[2][1] * mat.m[3][3]
		- mat.m[0][0] * mat.m[1][1] * mat.m[2][3] * mat.m[3][2]
		- mat.m[0][1] * mat.m[1][0] * mat.m[2][2] * mat.m[3][3]
		- mat.m[0][2] * mat.m[1][0] * mat.m[2][3] * mat.m[3][1]
		- mat.m[0][3] * mat.m[1][0] * mat.m[2][1] * mat.m[3][2]
		+ mat.m[0][3] * mat.m[1][0] * mat.m[2][2] * mat.m[3][1]
		+ mat.m[0][2] * mat.m[1][0] * mat.m[2][1] * mat.m[3][3]
		+ mat.m[0][1] * mat.m[1][0] * mat.m[2][3] * mat.m[3][2]
		+ mat.m[0][1] * mat.m[1][2] * mat.m[2][0] * mat.m[3][3]
		+ mat.m[0][2] * mat.m[1][3] * mat.m[2][0] * mat.m[3][1]
		+ mat.m[0][3] * mat.m[1][1] * mat.m[2][0] * mat.m[3][2]
		- mat.m[0][3] * mat.m[1][2] * mat.m[2][0] * mat.m[3][1]
		- mat.m[0][2] * mat.m[1][1] * mat.m[2][0] * mat.m[3][3]
		- mat.m[0][1] * mat.m[1][3] * mat.m[2][0] * mat.m[3][2]
		- mat.m[0][1] * mat.m[1][2] * mat.m[2][3] * mat.m[3][0]
		- mat.m[0][2] * mat.m[1][3] * mat.m[2][1] * mat.m[3][0]
		- mat.m[0][3] * mat.m[1][1] * mat.m[2][2] * mat.m[3][0]
		+ mat.m[0][3] * mat.m[1][2] * mat.m[2][1] * mat.m[3][0]
		+ mat.m[0][2] * mat.m[1][1] * mat.m[2][3] * mat.m[3][0]
		+ mat.m[0][1] * mat.m[1][3] * mat.m[2][2] * mat.m[3][0];

	return {
		(mat.m[1][1] * mat.m[2][2] * mat.m[3][3] + mat.m[1][2] * mat.m[2][3] * mat.m[3][1] + mat.m[1][3] * mat.m[2][1] * mat.m[3][2]
			- mat.m[1][3] * mat.m[2][2] * mat.m[3][1] - mat.m[1][2] * mat.m[2][1] * mat.m[3][3] - mat.m[1][1] * mat.m[2][3] * mat.m[3][2]) / d,
		-(mat.m[0][1] * mat.m[2][2] * mat.m[3][3] + mat.m[0][2] * mat.m[2][3] * mat.m[3][1] + mat.m[0][3] * mat.m[2][1] * mat.m[3][2]
			- mat.m[0][3] * mat.m[2][2] * mat.m[3][1] - mat.m[0][2] * mat.m[2][1] * mat.m[3][3] - mat.m[0][1] * mat.m[2][3] * mat.m[3][2]) / d,
		(mat.m[0][1] * mat.m[1][2] * mat.m[3][3] + mat.m[0][2] * mat.m[1][3] * mat.m[3][1] + mat.m[0][3] * mat.m[1][1] * mat.m[3][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[3][1] - mat.m[0][2] * mat.m[1][1] * mat.m[3][3] - mat.m[0][1] * mat.m[1][3] * mat.m[3][2]) / d,
		-(mat.m[0][1] * mat.m[1][2] * mat.m[2][3] + mat.m[0][2] * mat.m[1][3] * mat.m[2][1] + mat.m[0][3] * mat.m[1][1] * mat.m[2][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[2][1] - mat.m[0][2] * mat.m[1][1] * mat.m[2][3] - mat.m[0][1] * mat.m[1][3] * mat.m[2][2]) / d,

		-(mat.m[1][0] * mat.m[2][2] * mat.m[3][3] + mat.m[1][2] * mat.m[2][3] * mat.m[3][0] + mat.m[1][3] * mat.m[2][0] * mat.m[3][2]
			- mat.m[1][3] * mat.m[2][2] * mat.m[3][0] - mat.m[1][2] * mat.m[2][0] * mat.m[3][3] - mat.m[1][0] * mat.m[2][3] * mat.m[3][2]) / d,
		(mat.m[0][0] * mat.m[2][2] * mat.m[3][3] + mat.m[0][2] * mat.m[2][3] * mat.m[3][0] + mat.m[0][3] * mat.m[2][0] * mat.m[3][2]
			- mat.m[0][3] * mat.m[2][2] * mat.m[3][0] - mat.m[0][2] * mat.m[2][0] * mat.m[3][3] - mat.m[0][0] * mat.m[2][3] * mat.m[3][2]) / d,
		-(mat.m[0][0] * mat.m[1][2] * mat.m[3][3] + mat.m[0][2] * mat.m[1][3] * mat.m[3][0] + mat.m[0][3] * mat.m[1][0] * mat.m[3][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[3][0] - mat.m[0][2] * mat.m[1][0] * mat.m[3][3] - mat.m[0][0] * mat.m[1][3] * mat.m[3][2]) / d,
		(mat.m[0][0] * mat.m[1][2] * mat.m[2][3] + mat.m[0][2] * mat.m[1][3] * mat.m[2][0] + mat.m[0][3] * mat.m[1][0] * mat.m[2][2]
			- mat.m[0][3] * mat.m[1][2] * mat.m[2][0] - mat.m[0][2] * mat.m[1][0] * mat.m[2][3] - mat.m[0][0] * mat.m[1][3] * mat.m[2][2]) / d,

		(mat.m[1][0] * mat.m[2][1] * mat.m[3][3] + mat.m[1][1] * mat.m[2][3] * mat.m[3][0] + mat.m[1][3] * mat.m[2][0] * mat.m[3][1]
			- mat.m[1][3] * mat.m[2][1] * mat.m[3][0] - mat.m[1][1] * mat.m[2][0] * mat.m[3][3] - mat.m[1][0] * mat.m[2][3] * mat.m[3][1]) / d,
		-(mat.m[0][0] * mat.m[2][1] * mat.m[3][3] + mat.m[0][1] * mat.m[2][3] * mat.m[3][0] + mat.m[0][3] * mat.m[2][0] * mat.m[3][1]
			- mat.m[0][3] * mat.m[2][1] * mat.m[3][0] - mat.m[0][1] * mat.m[2][0] * mat.m[3][3] - mat.m[0][0] * mat.m[2][3] * mat.m[3][1]) / d,
		(mat.m[0][0] * mat.m[1][1] * mat.m[3][3] + mat.m[0][1] * mat.m[1][3] * mat.m[3][0] + mat.m[0][3] * mat.m[1][0] * mat.m[3][1]
			- mat.m[0][3] * mat.m[1][1] * mat.m[3][0] - mat.m[0][1] * mat.m[1][0] * mat.m[3][3] - mat.m[0][0] * mat.m[1][3] * mat.m[3][1]) / d,
		-(mat.m[0][0] * mat.m[1][1] * mat.m[2][3] + mat.m[0][1] * mat.m[1][3] * mat.m[2][0] + mat.m[0][3] * mat.m[1][0] * mat.m[2][1]
			- mat.m[0][3] * mat.m[1][1] * mat.m[2][0] - mat.m[0][1] * mat.m[1][0] * mat.m[2][3] - mat.m[0][0] * mat.m[1][3] * mat.m[2][1]) / d,

		-(mat.m[1][0] * mat.m[2][1] * mat.m[3][2] + mat.m[1][1] * mat.m[2][2] * mat.m[3][0] + mat.m[1][2] * mat.m[2][0] * mat.m[3][1]
			- mat.m[1][2] * mat.m[2][1] * mat.m[3][0] - mat.m[1][1] * mat.m[2][0] * mat.m[3][2] - mat.m[1][0] * mat.m[2][2] * mat.m[3][1]) / d,
		(mat.m[0][0] * mat.m[2][1] * mat.m[3][2] + mat.m[0][1] * mat.m[2][2] * mat.m[3][0] + mat.m[0][2] * mat.m[2][0] * mat.m[3][1]
			- mat.m[0][2] * mat.m[2][1] * mat.m[3][0] - mat.m[0][1] * mat.m[2][0] * mat.m[3][2] - mat.m[0][0] * mat.m[2][2] * mat.m[3][1]) / d,
		-(mat.m[0][0] * mat.m[1][1] * mat.m[3][2] + mat.m[0][1] * mat.m[1][2] * mat.m[3][0] + mat.m[0][2] * mat.m[1][0] * mat.m[3][1]
			- mat.m[0][2] * mat.m[1][1] * mat.m[3][0] - mat.m[0][1] * mat.m[1][0] * mat.m[3][2] - mat.m[0][0] * mat.m[1][2] * mat.m[3][1]) / d,
		(mat.m[0][0] * mat.m[1][1] * mat.m[2][2] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0] + mat.m[0][2] * mat.m[1][0] * mat.m[2][1]
			- mat.m[0][2] * mat.m[1][1] * mat.m[2][0] - mat.m[0][1] * mat.m[1][0] * mat.m[2][2] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1]) / d
	};
}

Matrix4x4 Matrix::MakeIdentity4x4() {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeTranslationMatrix(const Vector3& pos) {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		pos.x, pos.y, pos.z, 1.0f
	};
}

Matrix4x4 Matrix::MakeRotationMatrix(Vector3 angle) {
	Matrix4x4 x = {
		1.0f ,0.0f, 0.0f, 0.0f,
		0.0f, std::cosf(angle.x), std::sinf(angle.x), 0.0f,
		0.0f, -std::sinf(angle.x), std::cosf(angle.x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4x4 y = {
		std::cosf(angle.y), 0.0f, -std::sinf(angle.y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sinf(angle.y), 0.0f, std::cosf(angle.y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4x4 z = {
		std::cosf(angle.z), std::sinf(angle.z), 0.0f, 0.0f,
		-sinf(angle.z), std::cosf(angle.z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return x * y * z;
}

Matrix4x4 Matrix::MakeScaleMatrix(const Vector3& scale) {
	return {
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix::MakeAffineMatrix(const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
	return MakeTranslationMatrix(translation) *
		MakeRotationMatrix(rotation) *
		Matrix::MakeScaleMatrix(scale);
}

Matrix4x4 Matrix::MakeAffineMatrix(Transform transform) {
	return MakeAffineMatrix(transform.position, transform.rotation, transform.scale);
}
