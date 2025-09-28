#pragma once
#include <cmath>

struct Vector2 final {
	float x;
	float y;

	float Length() const {
		return sqrtf(x * x + y * y);
	}

	Vector2 Normalize() const {
		float len = Length();
		if (len == 0.0f) {
			return { 0.0f, 0.0f };
		}
		return { x / len, y / len };
	}

	float& operator[](int i) {
		return *(&x + i);
	}
};

struct Vector3 final {
	float x;
	float y;
	float z;

	float Length() const {
		return sqrtf(x * x + y * y + z * z);
	}

	Vector3 Normalize() const {
		float len = Length();
		if (len == 0.0f) {
			return { 0.0f, 0.0f, 0.0f };
		}
		return { x / len, y / len, z / len };
	}

	float& operator[](int i) {
		return *(&x + i);
	}
};

struct Vector4 final {
	float x;
	float y;
	float z;
	float w;

	float Length() const {
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	Vector4 Normalize() const {
		float len = Length();
		if (len == 0.0f) {
			return { 0.0f, 0.0f, 0.0f, 0.0f };
		}
		return { x / len, y / len, z / len, w / len };
	}

	float& operator[](int i) {
		return *(&x + i);
	}
};
