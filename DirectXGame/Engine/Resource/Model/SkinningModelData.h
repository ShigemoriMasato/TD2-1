#pragma once
#include "ModelData.h"

class SkinningModelData : public ModelData {
public:
	SkinningModelData() = default;
	~SkinningModelData() = default;

	void LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager) override;

	Node GetNode() { return rootNode_; }

private:

};
