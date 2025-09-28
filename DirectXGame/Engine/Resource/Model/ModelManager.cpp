#include "ModelManager.h"
#include <filesystem>

namespace fs = std::filesystem;

ModelManager::ModelManager(TextureManager* textureManager) {
	textureManager_ = textureManager;
}

ModelManager::~ModelManager() {
}

int ModelManager::LoadModel(const std::string& directoryPath) {
	if(modelHandleMap_.find(directoryPath) != modelHandleMap_.end()) {
		return modelHandleMap_[directoryPath];
	}

	models_.push_back(std::make_unique<ModelData>());
	fs::path dirPath(directoryPath);
	models_.back()->LoadModel(basePath_ + directoryPath, dirPath.filename().string() + ".obj", textureManager_);

	return int(models_.size() - 1);
}

ModelData* ModelManager::GetModelData(int modelHandle) {
	if (modelHandle < 0 || modelHandle >= static_cast<int>(models_.size())) {
		return nullptr;
	}
	return models_[modelHandle].get();
}
