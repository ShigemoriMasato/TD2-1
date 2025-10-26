#include "ScoreManager.h"
#include "LevelLoader.h"

ScoreManager::ScoreManager() {
	scores_.resize(LevelLoader::GetLevelFileMap().size(), 0);

	binaryManager_ = std::make_unique<BinaryManager>();
	auto values = binaryManager_->Read(saveFile);
	for (size_t i = 0; i < values.size() && i < scores_.size(); ++i) {
		scores_[i] = BinaryManager::Reverse<int>(values[i]);
	}
}

ScoreManager::~ScoreManager() {
	for (const auto& score : scores_) {
		binaryManager_->RegistOutput<int>(score, "");
	}
	binaryManager_->Write(saveFile);
}

void ScoreManager::SaveScore(int stage, int score) {
	scores_[stage] = score;
}

std::vector<int> ScoreManager::GetAllScores() {
	return scores_;
}

int ScoreManager::GetScore(int stage) {
	return scores_[stage];
}
