#pragma once
#include <Windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <wrl.h>
#include <mutex>
#include "Engine/Sound/Audio.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Camera/Camera.h"
#include <memory>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

struct CoTaskMemFreeDeleter {
	void operator()(WAVEFORMATEX* p) const {
		if (p) CoTaskMemFree(p);
	}
};

enum class Waveform {
	Time,
	Frequency
};

class SoundWave {
public:
	SoundWave();
	~SoundWave();

	void AudioCapture(Waveform waveform);

	void Draw(Camera* camera);

	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}

	std::vector<float> GetWaveform();

	Transform transform_{};

private:

	void WaveCapture();
	void ConvertFrequency();

	ComPtr<IMMDeviceEnumerator> enumerator_ = nullptr;

    ComPtr<IMMDevice> device_ = nullptr;
    ComPtr<IAudioClient> audioClient_ = nullptr;
    ComPtr<IAudioCaptureClient> captureClient_ = nullptr;
	std::unique_ptr<WAVEFORMATEX, CoTaskMemFreeDeleter> wfx_;

	std::unique_ptr<Logger> logger_ = nullptr;

	std::vector<float> audioWaveform_;
	std::mutex audioWaveformMutex_;

	static const size_t WAVEFORM_DISPLAY_SAMPLES_ = 512;
};
