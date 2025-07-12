#pragma once
#include "../Sound/Audio.h"
#include <vector>

class Sound {
public:
	Sound();
	~Sound();

	int LoadAudio(const char* filename, bool isBGM = false);
	void PlayAudio();

	static std::vector<bool> se;
	static std::vector<bool> bgm;
	static int GetSampleRate(int index);

private:

	static std::vector<int> seIndex_;
	static std::vector<int> bgmIndex_;

	static std::vector<bool> preBGM_;

	static int bgmHandle_;

	static bool isBGMPlay_;

	static Audio* audio_;
};

