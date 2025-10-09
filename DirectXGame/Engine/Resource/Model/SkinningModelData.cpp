#include "SkinningModelData.h"

#include <assimp/postprocess.h>

void SkinningModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager) {
	Assimp::Importer importer;
	std::string path = (directoryPath + "/" + filename);
	const aiScene* scene = nullptr;
	scene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder);

}
