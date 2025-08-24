#pragma once
#include <Data/Transform.h>
#include <Data/DrawData.h>
#include <Core/PSO/PSOConfig.h>

struct DrawConfig {
	int textureID;
	MaterialData material;
	PSOConfig pso;
	TramsformMatrixData matrixData;

	bool operator==(const DrawConfig& other) const {
		return textureID == other.textureID && material == other.material && pso == other.pso && matrixData == other.matrixData;
	}
};

namespace std {
	template<>
	struct hash<DrawConfig> {
		size_t operator()(const DrawConfig& cfg) const {
			size_t h = 0;
			hash_combine(h, hash<int>()(cfg.textureID));
			hash_combine(h, hash<MaterialData>()(cfg.material));
			hash_combine(h, hash<PSOConfig>()(cfg.pso));
			hash_combine(h, hash<TramsformMatrixData>()(cfg.matrixData));
			return h;
		}

	private:
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}
