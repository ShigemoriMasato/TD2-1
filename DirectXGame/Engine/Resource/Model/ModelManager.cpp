#include "ModelManager.h"
#include <filesystem>

namespace fs = std::filesystem;

ModelManager::ModelManager(TextureManager* textureManager, DXDevice* device, SRVManager* srvManager) {
	textureManager_ = textureManager;
	device_ = device;
	srvManager_ = srvManager;
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

		auto glbfile = SearchFiles(basePath_ + directoryPath, ".glb");

		//glbも見つからなかったら
		if (glbfile.size() != 1) {

			auto gltffile = SearchFiles(basePath_ + directoryPath, ".gltf");

			//gltfも見つからなかったら
			if(gltffile.size() != 1) {
			assert(false && "Can't find .obj or glb file");
			return -1;
			}

			//gltfが見つかったら
			glbfile = gltffile;
		}

		models_.push_back(std::make_unique<ModelData>());
		models_.back()->LoadModel(basePath_ + directoryPath, glbfile[0], textureManager_, device_, srvManager_);
		
		animations_[static_cast<int>(models_.size() - 1)] = Animation();
		animations_[static_cast<int>(models_.size() - 1)] = LoadAnimationFile(basePath_ + directoryPath, glbfile[0]);

	} else {

		//モデルの読み込み
		models_.push_back(std::make_unique<ModelData>());
		models_.back()->LoadModel(basePath_ + directoryPath, objfile[0], textureManager_, device_, srvManager_);

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

Animation ModelManager::GetAnimation(int modelHandle) {
	if(animations_.find(modelHandle) == animations_.end()) {
		return Animation();
	}
	return animations_[modelHandle];
}
