#pragma once
#include "ModelData.h"
#include "ModelAnimation.h"

class ModelManager {
public:

	ModelManager(TextureManager* textureManager, DXDevice* device, SRVManager* srvManager);
	~ModelManager();

	int LoadModel(const std::string& directoryPath);

	ModelData* GetModelData(int modelHandle);
	Animation GetAnimation(int modelHandle);

private:

	std::unordered_map<std::string, int> modelHandleMap_;
	std::vector<std::unique_ptr<ModelData>> models_;

	std::unordered_map<int, Animation> animations_;

	const std::string basePath_ = "Assets/Model/";

	TextureManager* textureManager_ = nullptr;
	DXDevice* device_ = nullptr;
	SRVManager* srvManager_ = nullptr;
};

