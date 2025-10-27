#include "CommonData.h"

CommonData::CommonData() {
	keyManager_ = std::make_unique<KeyManager>();
	keyManager_->Initialize();

	scoreManager_ = std::make_unique<ScoreManager>();
}

CommonData::~CommonData() {
}
