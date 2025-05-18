#pragma once

struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3 operator*=(const float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3 operator*(const float scalar) const {
		Vector3 result = *this;
		result *= scalar;
		return result;
	}

	bool operator==(const Vector3& other) const {
		return x == other.x || y == other.y || z == other.z;
	}
};
