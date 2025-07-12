#include "Audio.h"
#include <cassert>
#include <fstream>

void Audio::Initialize() {
	HRESULT hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(hr));
}

int Audio::SoundLoadWave(const char* filename) {
	std::ifstream file;
	file.open(filename, std::ios::binary);
	assert(file.is_open() && "Audio::SoundLoadWave cannot open the wave file");

	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	FormatChunk format = {};

	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	if (strncmp(data.id, "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);

		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	SoundData soundData;

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.buffferSize = data.size;

	sdList_.push_back(soundData);

	return static_cast<int>(sdList_.size() - 1);
}

void Audio::SoundUnload(int soundHandle) {
	delete[] sdList_[soundHandle].pBuffer; //バッファを解放

	sdList_.erase(sdList_.begin() + soundHandle); //SoundDataを削除
}

int Audio::PlayWave(int soundHandle) {
	HRESULT hr;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, &sdList_[soundHandle].wfex);
	assert(SUCCEEDED(hr) && "Audio::SoundPlayWave CreateSourceVoice failed");

	XAUDIO2_BUFFER buffer = {};
	buffer.pAudioData = sdList_[soundHandle].pBuffer; //オーディオデータのポインタ
	buffer.AudioBytes = sdList_[soundHandle].buffferSize; //バッファのサイズ
	buffer.Flags = XAUDIO2_END_OF_STREAM; //ストリームの終端を示すフラグ

	hr = pSourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hr) && "Audio::SoundPlayWave SubmitSourceBuffer failed");
	hr = pSourceVoice->Start();

	int index = 0;
	for (auto& sv : sourceVoices_) {
		XAUDIO2_VOICE_STATE state = {};
		sv->GetState(&state);
		if (state.BuffersQueued == 0) {
			sv->DestroyVoice();
			sv = pSourceVoice; // 既存のソースボイスが空いている場合は再利用
			return index;
		}

		++index;
	}

	//再利用できなかったら新しく挿入する。
	sourceVoices_.push_back(pSourceVoice);

	return index;
}

void Audio::StopWave(int soundHandle) {
	sourceVoices_[soundHandle]->Stop();
}

bool Audio::IsPlayWave(int soundHandle) {
	XAUDIO2_VOICE_STATE state = {};
	sourceVoices_[soundHandle]->GetState(&state);
	return state.BuffersQueued != 0;
}

int Audio::GetSampleRate(int index) {
	return sdList_[index].wfex.nSamplesPerSec;
}
