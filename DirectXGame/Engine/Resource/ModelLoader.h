#pragma once
#include <string>
#include <vector>
#include <Data/Transform.h>
#include <Data/DrawData.h>
#include "Data/ModelManager.h"
#include "DrawResourceManager.h"

class ModelLoader {
public:

	ModelLoader(ModelManager* manager);
	~ModelLoader() = default;

	int LoadModel(const char* filePath);

private:

	std::vector<ModelMaterial> LoadMaterialTemplateFile(std::string filePath);

	ModelManager* modelManager_;
	DrawResourceManager* drawResourceManager_;

	std::string basePath_;
};
