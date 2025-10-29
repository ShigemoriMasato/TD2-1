#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Game/LevelLoader.h>
#include <Game/ScoreManager.h>

class CommonData {
public:

	CommonData();
	~CommonData();

	std::unique_ptr<KeyManager> keyManager_ = nullptr;

	bool isPushClose_ = false;

	std::unique_ptr<ScoreManager> scoreManager_ = nullptr;

	std::vector<std::vector<Vector3>> deathPoints_;
	std::vector<int> bestScore_;

	LevelIndex nextLevelIndex_ = LevelIndex::Level1;
	bool isCreateTexture = false;

	int bgmPlayHandle_ = -1;

private:

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	std::string saveFile = "data.sav";
};
