#include "CommonData.h"

CommonData::CommonData() {
	keyManager_ = std::make_unique<KeyManager>();
	keyManager_->Initialize();

	deathPoints_.resize(int(LevelIndex::kNumLevels));

	scoreManager_ = std::make_unique<ScoreManager>();

	binaryManager_ = std::make_unique<BinaryManager>();

	int levelNum = static_cast<int>(LevelIndex::kNumLevels);

	auto values = binaryManager_->Read(saveFile);
	size_t i;

	for (i = 0; i < values.size() && i < levelNum; ++i) {
		scoreManager_->SaveScore((int)i, BinaryManager::Reverse<int>(values[i]));
	}

	//安全装置
	if (values.size() > i)
	for (int stageIndex = 0; stageIndex < levelNum; ++stageIndex) {
		int size = BinaryManager::Reverse<int>(values[i++]);
		
		for (int j = 0; j < size; ++j) {
			deathPoints_[stageIndex].push_back(BinaryManager::Reverse<Vector3>(values[i++]));
		}

		//安全装置
		if (values.size() <= i) break;
	}
}

CommonData::~CommonData() {

	auto scores = scoreManager_->GetAllScores();

	for (int i = 0; i < scores.size(); ++i) {
		binaryManager_->RegistOutput(scores[i], "");
	}

	int levelNum = static_cast<int>(LevelIndex::kNumLevels);

	for (int stageIndex = 0; stageIndex < levelNum; ++stageIndex) {
		binaryManager_->RegistOutput(static_cast<int>(deathPoints_[stageIndex].size()), "");
	
		for (const auto& point : deathPoints_[stageIndex]) {
			binaryManager_->RegistOutput(point, "");
		}
	}

#ifdef SH_RELEASE
	binaryManager_->Write(saveFile);
#endif
}
