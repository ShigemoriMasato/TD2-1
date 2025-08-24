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

namespace std {
	template<>
	struct hash<Vector4> {
		size_t operator()(const Vector4& v) const {
			size_t h = 0;
			hash_combine(h, hash<float>()(v.x));
			hash_combine(h, hash<float>()(v.y));
			hash_combine(h, hash<float>()(v.z));
			hash_combine(h, hash<float>()(v.w));
			return h;
		}

	private:
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}

namespace std {
	template<>
	struct hash<Vector3> {
		size_t operator()(const Vector3& v) const {
			size_t h = 0;
			hash_combine(h, hash<float>()(v.x));
			hash_combine(h, hash<float>()(v.y));
			hash_combine(h, hash<float>()(v.z));
			return h;
		}

	private:
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}

namespace std {
	template<>
	struct hash<Vector2> {
		size_t operator()(const Vector2& v) const {
			size_t h = 0;
			hash_combine(h, hash<float>()(v.x));
			hash_combine(h, hash<float>()(v.y));
			return h;
		}

	private:
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}
