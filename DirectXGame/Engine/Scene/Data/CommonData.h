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

	LevelIndex nextLevelIndex_ = LevelIndex::Level1;
	bool isCreateTexture = false;

private:

};
