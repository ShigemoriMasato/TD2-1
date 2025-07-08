#include "BPMMeasure.h"
#include "Engine/Sound/Sound.h"
#include <numeric>

void BPMMeasure::Measure() {

    AudioCapture(Waveform::Time);

	if (!isMeasuring_) {
		return;
	}

    //音のサンプルを受け取る
    UpdateFromSource();

    //サンプルが集まるまで何もしない
    if (frame_++ < sanpleSeconds * 60) {
        return;
    }

    std::vector<float> downGreadBuffer;
    for (int i = 0; i < waveformSample_.size(); i += 4) {
		downGreadBuffer.push_back(waveformSample_[i]);
    }
	waveformSample_ = std::move(downGreadBuffer);

    float mean = std::accumulate(waveformSample_.begin(), waveformSample_.end(), 0.0f) / waveformSample_.size();
    for (auto& v : waveformSample_) {
        v -= mean;
    }

    float maxVal = *std::max_element(waveformSample_.begin(), waveformSample_.end());
    if (maxVal > 0.0f) {
        for (auto& v : waveformSample_) {
            v /= maxVal;
        }
    }

    std::vector<float> autocorr(maxLag);

	// 自己相関を計算する
    for (int lag = minLag; lag < maxLag; ++lag) {
        float sum = 0.0f;
        for (int i = 0; i + lag < waveformSample_.size(); ++i) {
            sum += waveformSample_[i] * waveformSample_[i + lag];
        }
        autocorr[lag] = sum;
    }

	// 最大の自己相関を見つける
    int bestLag = static_cast<int>(std::distance(autocorr.begin(), std::max_element(autocorr.begin() + minLag, autocorr.begin() + maxLag)));
    float bpm = 60.0f * sampleRate_ / bestLag;
	bpmBuffer_.push_back(bpm);

    // 測定回数の上限に達していない場合は終了
	if (++measureCount_ < measureMaxCount_) {
        frame_ = 0;
		return; 
	}

	//今まで計測したBPMの平均を計算する
    float sum{};
	for (const auto& b : bpmBuffer_) {
		sum += b;
	}
	bpm_ = sum / bpmBuffer_.size();

    isMeasuring_ = false;
    canOutputBPM_ = true;
}

void BPMMeasure::MeasureStart(const int index) {
    //音を流す
    Sound::isPlay_[index] = true;

    //サンプル数の取得
    sampleRate_ = Sound::GetSampleRate(index);
    bufferSample_ = sampleRate_ * sanpleSeconds;

    //ラグの設定
	minLag = sampleRate_ * 60 / 180; // 300BPMのラグ
	maxLag = sampleRate_ * 60 / 60; // 30BPMのラグ

    //フラグ系
	isMeasuring_ = true;
    canOutputBPM_ = false;
}

bool BPMMeasure::GetIsMeasuring() {
	return isMeasuring_;
}

void BPMMeasure::UpdateFromSource() {
    std::vector<float> newWave = GetWaveform();

    waveformSample_.insert(waveformSample_.end(), newWave.begin(), newWave.end());

    if (waveformSample_.size() > bufferSample_) {
        size_t excess = waveformSample_.size() - bufferSample_;
        waveformSample_.erase(waveformSample_.begin(), waveformSample_.begin() + excess);
    }
}

float BPMMeasure::GetBPM() {
    if (isMeasuring_ && !canOutputBPM_) {
        return -1.0f;
    }

    canOutputBPM_ = false;

    return bpm_;
}

void BPMMeasure::DrawWave(Camera* camera) {
    Draw(camera);
}
