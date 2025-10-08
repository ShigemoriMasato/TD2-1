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

	auto objfile = SearchFiles(basePath_ + directoryPath, ".obj");

	//objが見つからなかったら
	if (objfile.size() != 1) {

		auto glbfile = SearchFiles(basePath_ + directoryPath, ".gltf");

		//glbも見つからなかったら
		if (glbfile.size() != 1) {
			assert(false && "Can't find .obj or glb file");
			return -1;
		}

		//glbが見つかったら
		LoadGlbFile(directoryPath, glbfile[0]);

	} else {

		//objが見つかったら
		LoadObjFile(directoryPath, objfile[0]);

	}

	this->modelHandleMap_[directoryPath] = int(models_.size() - 1);

	return int(models_.size() - 1);
}

ModelData* ModelManager::GetModelData(int modelHandle) {
	if (modelHandle < 0 || modelHandle >= static_cast<int>(models_.size())) {
		return nullptr;
	}
	return models_[modelHandle].get();
}

void ModelManager::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	models_.push_back(std::make_unique<ModelData>());
	models_.back()->LoadModel(basePath_ + directoryPath, filename, textureManager_);
}

void ModelManager::LoadGlbFile(const std::string& directoryPath, const std::string& filename) {
	models_.push_back(std::make_unique<SkinningModelData>());
	models_.back()->LoadModel(basePath_ + directoryPath, filename, textureManager_);
}
