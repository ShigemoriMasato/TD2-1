#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Game/LevelLoader.h>

class CommonData {
public:

	CommonData();
	~CommonData();

	std::unique_ptr<KeyManager> keyManager_ = nullptr;

	bool isPushClose_ = false;

	LevelIndex nextLevelIndex_ = LevelIndex::Level1;
	bool isCreateTexture = false;

private:

};
