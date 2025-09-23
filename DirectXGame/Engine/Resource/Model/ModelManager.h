#pragma once
#include "ModelData.h"

class ModelManager {
public:

	ModelManager(TextureManager* textureManager);
	~ModelManager();

	int LoadModel(const std::string& directoryPath);

	ModelData* GetModelData(int modelHandle);

private:

	std::unordered_map<std::string, int> modelHandleMap_;
	std::vector<std::unique_ptr<ModelData>> models_;

	TextureManager* textureManager_ = nullptr;

};

