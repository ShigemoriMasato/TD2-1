#pragma once
#include <cmath>

struct Vector3;
struct Vector2 final {
	float x;
	float y;

    Vector2() :x(0.0f), y(0.0f) {}
	Vector2(float x_, float y_) :x(x_), y(y_) {}
	Vector2(const Vector3& v);

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
	Vector3() :x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float x_, float y_, float z_) :x(x_), y(y_), z(z_) {}
	Vector3(const Vector2& v);

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

inline Vector3::Vector3(const Vector2& v) {
	x = v.x;
	y = v.y;
	z = 0.0f;
}

inline Vector2::Vector2(const Vector3& v) {
	x = v.x;
	y = v.y;
}
