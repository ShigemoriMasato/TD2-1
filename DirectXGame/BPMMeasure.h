#pragma once
#include "SoundWave.h"
#include <fftw3.h>

class BPMMeasure : SoundWave {
public:

	BPMMeasure();
	~BPMMeasure();

	void Measure();

	void MeasureStart(const int index);

	bool GetIsMeasuring();

	float GetBPM();

	void ImGuiDraw();
	void DrawWave(Camera* camera);

private:

	void UpdateFromSource();

	std::vector<float> waveformSample_; // BPM測定用のサンプル
	std::vector<float> bpmBuffer_;

	float bpm_ = -1.0f;

	int maxLag = 1500; // 最大ラグ
	int minLag = 200; // 最小ラグ

	const int sanpleSeconds = 5;

	int sampleRate_ = 0;
	int bufferSample_ = 48000;

	int frame_ = 0;
	int measureCount_ = 0;
	const int measureMaxCount_ = 100; // 測定回数の上限

	bool isMeasuring_ = false;
	bool canOutputBPM_ = false;

	//fftw関連
	// FFTW用
	int fftSize_ = 0;
	fftw_plan fftPlanForward_ = nullptr;
	fftw_plan fftPlanBackward_ = nullptr;
	double* fftInput_ = nullptr;
	fftw_complex* fftFreq_ = nullptr;
	double* fftAutocorr_ = nullptr;
};
