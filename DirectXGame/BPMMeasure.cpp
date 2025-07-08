#include "BPMMeasure.h"
#include "Engine/Sound/Sound.h"
#include <numeric>
#include <fftw3.h>

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

	// 自己相関を計算する
    int N = static_cast<int>(waveformSample_.size());
    int paddedSize = 2 * N;

    // 入力をゼロパディング
    std::vector<double> input(paddedSize, 0.0);
    for (int i = 0; i < N; ++i) {
        input[i] = static_cast<double>(waveformSample_[i]);
    }

    // FFTWの入力・出力バッファ
    fftw_complex* freq = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (paddedSize / 2 + 1));
    double* autocorr_raw = (double*)fftw_malloc(sizeof(double) * paddedSize);

    // FFTと逆FFTのプラン作成
    fftw_plan forward = fftw_plan_dft_r2c_1d(paddedSize, input.data(), freq, FFTW_ESTIMATE);
    fftw_plan backward = fftw_plan_dft_c2r_1d(paddedSize, freq, autocorr_raw, FFTW_ESTIMATE);

    // FFT実行
    fftw_execute(forward);

    // パワースペクトル（自己相関の周波数領域表現）を作る
    for (int i = 0; i < paddedSize / 2 + 1; ++i) {
        double real = freq[i][0];
        double imag = freq[i][1];
        freq[i][0] = real * real + imag * imag; // 実部にパワーを格納
        freq[i][1] = 0.0; // 虚部はゼロに
    }

    // 逆FFTで自己相関を得る
    fftw_execute(backward);

    // 正規化（FFTWはスケーリングしないので）
    for (int i = 0; i < paddedSize; ++i) {
        autocorr_raw[i] /= paddedSize;
    }

    // 自己相関の有効範囲だけ取り出す
    std::vector<double> autocorr(maxLag);
    for (int lag = minLag; lag < maxLag; ++lag) {
        autocorr[lag] = autocorr_raw[lag];
    }

    // ピークを探してBPMを計算
    int bestLag = static_cast<int>(std::distance(autocorr.begin(), std::max_element(autocorr.begin() + minLag, autocorr.begin() + maxLag)));
    float bpm = 60.0f * sampleRate_ / bestLag;
    bpmBuffer_.push_back(bpm);

    // FFTWの終了処理
    fftw_destroy_plan(forward);
    fftw_destroy_plan(backward);
    fftw_free(freq);
    fftw_free(autocorr_raw);

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
