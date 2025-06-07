#pragma once

struct Vector4 final {
	float x;
	float y;
	float z;
	float w;

	Vector4 operator+ (const Vector4& vec) const {
		return { x + vec.x, y + vec.y, z + vec.z, w + vec.w };
	}

	Vector4 operator- (const Vector4& vec) const {
		return { x - vec.x, y - vec.y, z - vec.z, w - vec.w };
	}

	Vector4 operator*(const float scalar) const {
		return { x * scalar, y * scalar, z * scalar, 1.0f };
	}
};
