#include "SkinningModelData.h"

#include <assimp/postprocess.h>

void SkinningModelData::LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager) {
	Assimp::Importer importer;
	std::string path = (directoryPath + "/" + filename);
	const aiScene* scene = nullptr;
	scene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder);

	LoadVertex(scene, directoryPath, textureManager);

	if (scene->hasSkeletons()) {
		rootNode_ = LoadNode(scene->mRootNode);
	}

}

Node SkinningModelData::LoadNode(aiNode* node) {
	Node result{};
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);

	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再帰的に子ノードも読み込む
		result.children[childIndex] = LoadNode(node->mChildren[childIndex]);
	}

	return result;
}
