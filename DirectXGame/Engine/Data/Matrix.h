#pragma once

struct Matrix3x3 {
	float m[3][3];
};

struct Matrix4x4 final {
	float m[4][4];
};

namespace std {
	template<>
	struct hash<Matrix4x4> {
		size_t operator()(const Matrix4x4& mat) const {
			size_t h = 0;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					hash_combine(h, hash<float>()(mat.m[i][j]));
				}
			}
			return h;
		}

	private:
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}
