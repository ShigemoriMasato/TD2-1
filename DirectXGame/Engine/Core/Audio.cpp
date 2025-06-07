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

	int index = 0;

	sdList_.push_back(soundData);

	return index; //インデックスを返す
}

void Audio::SoundUnload(int soundHandle) {
	delete[] sdList_[soundHandle].pBuffer; //バッファを解放

	sdList_.erase(sdList_.begin() + soundHandle); //SoundDataを削除
}

void Audio::SoundPlayWave(int soundHandle) {
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
}
