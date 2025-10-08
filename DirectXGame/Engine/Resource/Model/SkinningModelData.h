#pragma once
#include "ModelData.h"
#include <Math/MyMath.h>

struct Node {
	Matrix4x4 localMatrix = Matrix::MakeIdentity4x4();
	std::string name{};
	std::vector<Node> children{};
};

class SkinningModelData : public ModelData {
public:
	SkinningModelData() = default;
	~SkinningModelData() = default;

	void LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager) override;

	Node GetNode() { return rootNode_; }

private:

	Node LoadNode(aiNode* node);

	Node rootNode_{};

};
