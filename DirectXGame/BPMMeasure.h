#pragma once
#include "SoundWave.h"

class BPMMeasure : SoundWave {
public:

	BPMMeasure() = default;
	~BPMMeasure() = default;

	void Measure();

	void MeasureStart(const int index);

	bool GetIsMeasuring();

	float GetBPM();

	void DrawWave(Camera* camera);

private:

	void UpdateFromSource();

	std::vector<float> waveformSample_; // BPM測定用のサンプル
	std::vector<float> bpmBuffer_;

	float bpm_ = -1.0f;

	int maxLag = 1500; // 最大ラグ
	int minLag = 200; // 最小ラグ

	const int sanpleSeconds = 3;

	int sampleRate_ = 0;
	int bufferSample_ = 0;

	int frame_ = 0;
	int measureCount_ = 0;
	const int measureMaxCount_ = 5; // 測定回数の上限

	bool isMeasuring_ = false;
	bool canOutputBPM_ = false;
};
