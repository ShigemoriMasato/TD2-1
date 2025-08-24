#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <Math/MyMath.h>
#include <string>
#include <vector>

struct VertexData final {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData final {
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	int32_t enableLighting = true;
	float padding[3];
	Matrix4x4 uvTransform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f ,0.0f, 1.0f, 0.0f
	};

	bool operator==(const MaterialData& other) const {
		return (color == other.color) &&
			(enableLighting == other.enableLighting) &&
			(uvTransform == other.uvTransform);
	}
};

struct ModelMaterial final {
	std::string name;
	int textureHandle;
};

struct TramsformMatrixData final {
	Matrix4x4 wvp;
	Matrix4x4 world;

	bool operator==(const TramsformMatrixData& other) const {
		return (wvp == other.wvp) && (world == other.world);
	}
};

struct DirectionalLightData final {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct ModelData final {
	std::unordered_map<std::string, std::vector<VertexData>> vertices;
	std::vector<ModelMaterial> material{};
};

namespace std {
	template<>
	struct hash<MaterialData> {
		size_t operator()(const MaterialData& m) const {
			size_t h = 0;
			hash_combine(h, hash<Vector4>()(m.color));
			hash_combine(h, hash<int32_t>()(m.enableLighting));
			hash_combine(h, hash<Matrix4x4>()(m.uvTransform));
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
	struct hash<TramsformMatrixData> {
		size_t operator()(const TramsformMatrixData& m) const {
			size_t h = 0;
			hash_combine(h, hash<Matrix4x4>()(m.wvp));
			hash_combine(h, hash<Matrix4x4>()(m.world));
			return h;
		}

	private:
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}
