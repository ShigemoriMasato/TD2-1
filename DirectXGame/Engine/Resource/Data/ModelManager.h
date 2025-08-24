#pragma once
#include <Data/DrawData.h>
#include <Resource/DrawResourceManager.h>

class ModelManager {
public:

	ModelManager() = default;
	~ModelManager() = default;

	int AddModelData(ModelData modelData);

	ModelData GetModelData(int handle);

private:

	std::vector<ModelData> modelDataList_;

};
