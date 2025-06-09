#pragma once
#include "../Engine/Core/Audio.h"
#include <vector>

class Sound {
public:
	Sound();
	~Sound();

	int LoadAudio(const char* filename);
	void PlayAudio();

	static std::vector<bool> isPlay_;

private:
	Audio* audio_;
};

