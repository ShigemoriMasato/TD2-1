#include "Sound.h"

std::vector<bool> Sound::isPlay_ = {};
Audio* Sound::audio_ = nullptr;

Sound::Sound() {
	audio_ = new Audio();
	audio_->Initialize();
}

Sound::~Sound() {
	delete audio_;
}

int Sound::LoadAudio(const char* filename) {
	isPlay_.push_back(false);
	return audio_->SoundLoadWave(filename);
}

void Sound::PlayAudio() {
	for (int i = 0; i < int(isPlay_.size()); ++i) {
		if (isPlay_[i]) {
			audio_->SoundPlayWave(i);
			isPlay_[i] = false;
			return;
		}
	}
}

int Sound::GetSampleRate(int index) {
	if (!audio_) {
		return -1;
	}

	return audio_->GetSampleRate(index);
}
