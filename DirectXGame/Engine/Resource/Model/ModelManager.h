#pragma once
#include "SkinningModelData.h"

class ModelManager {
public:

	ModelManager(TextureManager* textureManager);
	~ModelManager();

	int LoadModel(const std::string& directoryPath);

	ModelData* GetModelData(int modelHandle);

private:

	void LoadObjFile(const std::string& directoryPath, const std::string& filename);
	void LoadGlbFile(const std::string& directoryPath, const std::string& filename);

	std::unordered_map<std::string, int> modelHandleMap_;
	std::vector<std::unique_ptr<ModelData>> models_;

	const std::string basePath_ = "Assets/Model/";

	TextureManager* textureManager_ = nullptr;

};

