#include "Sound.h"

std::vector<bool> Sound::se = {};
std::vector<int> Sound::seIndex_ = {};
std::vector<bool> Sound::bgm = {};
std::vector<bool> Sound::preBGM_ = {};
std::vector<int> Sound::bgmIndex_ = {};
int Sound::bgmHandle_ = -1;
bool Sound::isBGMPlay_ = false;
Audio* Sound::audio_ = nullptr;

Sound::Sound() {
	audio_ = new Audio();
	audio_->Initialize();
}

Sound::~Sound() {
	delete audio_;
}

int Sound::LoadAudio(const char* filename, bool isBGM) {
	int index = audio_->SoundLoadWave(filename);
	if (isBGM) {
		bgm.push_back(false);
		preBGM_.push_back(false);
		bgmIndex_.push_back(index);
	} else {
		se.push_back(false);
		seIndex_.push_back(index);
	}

	return index;
}

void Sound::PlayAudio() {
	if (!audio_) {
		return;
	}
	for (size_t i = 0; i < seIndex_.size(); ++i) {
		if (se[i]) {
			audio_->PlayWave(seIndex_[i]);
			seIndex_[i] = false;
		}
	}

	bool isBGMPlaying = false;
	for (size_t i = 0; i < bgmIndex_.size(); ++i) {
		if (bgm[i] && !preBGM_[i]) {

			if (bgmHandle_ != -1) {
				audio_->StopWave(bgmHandle_);
			}

			bgmHandle_ = audio_->PlayWave(bgmIndex_[i]);
			preBGM_[i] = true;

			//他のBGMを停止する
			for (int j = 0; j < bgm.size(); ++j) {
				if (j != i) {
					if (bgm[j]) {
						bgm[j] = false;
						preBGM_[j] = false;
					}
				}
			}

			isBGMPlay_ = true;

			return;

		} else if (bgm[i] && !audio_->IsPlayWave(bgmHandle_)) {
			bgmHandle_ = audio_->PlayWave(bgmIndex_[i]);
			preBGM_[i] = true;
			isBGMPlay_ = true;
			return;
		} else if (bgm[i]) {
			isBGMPlaying = true;
		}
	}

	if (isBGMPlay_ && !isBGMPlaying) {
		audio_->StopWave(bgmHandle_);
		isBGMPlay_ = false;
	}
}

int Sound::GetSampleRate(int index) {
	if (!audio_) {
		return -1;
	}

	return audio_->GetSampleRate(index);
}
