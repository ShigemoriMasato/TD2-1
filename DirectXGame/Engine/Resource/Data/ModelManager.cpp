#include "ModelManager.h"

int ModelManager::AddModelData(ModelData modelData) {

	int index = static_cast<int>(modelDataList_.size());

	for (int i = 1; i < modelData.material.size(); ++i) {
		modelDataList_.push_back(ModelData());
	}

	return index - 1;
}

ModelData ModelManager::GetModelData(int handle) {
	return modelDataList_[handle];
}
