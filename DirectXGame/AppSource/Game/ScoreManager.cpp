#include "ScoreManager.h"
#include "LevelLoader.h"

ScoreManager::ScoreManager() {
	scores_.resize(LevelLoader::GetLevelFileMap().size(), 0);
}

ScoreManager::~ScoreManager() {
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
