#pragma once
#include "../Sound/Audio.h"
#include <vector>

class Sound {
public:
	Sound();
	~Sound();

	int LoadAudio(const char* filename);
	void PlayAudio();

	static std::vector<bool> isPlay_;
	static int GetSampleRate(int index);

private:
	static Audio* audio_;
};

