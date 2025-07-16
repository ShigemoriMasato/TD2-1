#include "BPMMeasure.h"
#include "Engine/Sound/Sound.h"
#include <numeric>
#include <fftw3.h>
#include <cassert>
#include "externals/imgui/imgui.h"

BPMMeasure::BPMMeasure() {
    
}

BPMMeasure::~BPMMeasure() {
	// FFTWのプランとメモリを解放
    if (fftPlanForward_) fftw_destroy_plan(fftPlanForward_);
    if (fftPlanBackward_) fftw_destroy_plan(fftPlanBackward_);
    if (fftInput_) fftw_free(fftInput_);
    if (fftFreq_) fftw_free(fftFreq_);
    if (fftAutocorr_) fftw_free(fftAutocorr_);
}

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

    // 平均除去
    float mean = std::accumulate(waveformSample_.begin(), waveformSample_.end(), 0.0f) / waveformSample_.size();
    for (auto& v : waveformSample_) {
        v -= mean;
    }

    // 振幅正規化
    float maxval = *std::max_element(waveformSample_.begin(), waveformSample_.end());
    if (maxval > 0.0f) {
        for (auto& v : waveformSample_) {
            v /= maxval;
        }
    }

    // waveformSample_ を fftInput_ にコピー＆ゼロパディング
    std::fill(fftInput_, fftInput_ + fftSize_, 0.0);
    for (int i = 0; i < waveformSample_.size(); ++i) {
        fftInput_[i] = waveformSample_[i];
    }

    // FFT実行
    fftw_execute(fftPlanForward_);

    // パワースペクトル（自己相関の周波数表現）
    for (int i = 0; i < fftSize_ / 2 + 1; ++i) {
        double real = fftFreq_[i][0];
        double imag = fftFreq_[i][1];
        fftFreq_[i][0] = real * real + imag * imag;
        fftFreq_[i][1] = 0.0;
    }

    // 逆FFTで自己相関を得る
    fftw_execute(fftPlanBackward_);

    // 正規化
    for (int i = 0; i < fftSize_; ++i) {
        fftAutocorr_[i] /= fftSize_;
    }

    // BPM推定（minLag〜maxLagの範囲で最大値を探す）
    int bestLag = minLag;
    double maxVal = fftAutocorr_[minLag];
    for (int lag = minLag + 1; lag < maxLag; ++lag) {
        if (fftAutocorr_[lag] > maxVal) {
            maxVal = fftAutocorr_[lag];
            bestLag = lag;
        }
    }

    float bpm = 60.0f * sampleRate_ / bestLag;
    bpmBuffer_.push_back(bpm);

    // 測定回数の上限に達していない場合は終了
	if (++measureCount_ < measureMaxCount_) {
        frame_ = sanpleSeconds * 60 - 20;
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

    // FFTWのプランとメモリを解放
    if (fftPlanForward_) fftw_destroy_plan(fftPlanForward_);
    fftPlanForward_ = nullptr;
    if (fftPlanBackward_) fftw_destroy_plan(fftPlanBackward_);
	fftPlanBackward_ = nullptr;
    if (fftInput_) fftw_free(fftInput_);
    fftInput_ = nullptr;
    if (fftFreq_) fftw_free(fftFreq_);
    fftFreq_ = nullptr;
    if (fftAutocorr_) fftw_free(fftAutocorr_);
    fftAutocorr_ = nullptr;
}

void BPMMeasure::MeasureStart(const int index) {
    //音を流す
    Sound::bgm[index] = true;

    //サンプル数の取得
    sampleRate_ = Sound::GetSampleRate(index);
    bufferSample_ = sampleRate_ * sanpleSeconds;

    //ラグの設定
	minLag = sampleRate_ * 60 / 200; // 200BPMのラグ
	maxLag = sampleRate_ * 60 / 60; // 30BPMのラグ

    //フラグ系
	isMeasuring_ = true;
    canOutputBPM_ = false;

	//fftwの初期化
    fftSize_ = 2 * bufferSample_;

    // メモリ確保
    fftInput_ = (double*)fftw_malloc(sizeof(double) * fftSize_);
    fftFreq_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (fftSize_ / 2 + 1));
    fftAutocorr_ = (double*)fftw_malloc(sizeof(double) * fftSize_);

    if (!fftInput_ || !fftFreq_ || !fftAutocorr_) {
        // メモリ確保に失敗したら安全に抜ける
        assert(false && "Failed to allocate memory for FFTW arrays");
        return;
    }

    // プラン作成
    fftPlanForward_ = fftw_plan_dft_r2c_1d(fftSize_, fftInput_, fftFreq_, FFTW_ESTIMATE);
    fftPlanBackward_ = fftw_plan_dft_c2r_1d(fftSize_, fftFreq_, fftAutocorr_, FFTW_ESTIMATE);
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

void BPMMeasure::ImGuiDraw() {
    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize({ 200, 720 });
    //ImGuiでbpmBufferを表示
    ImGui::Begin("BPM Buffer");
    for (int i = 0; i < bpmBuffer_.size(); ++i) {
        ImGui::Text("BPM[%d]: %f", i, bpmBuffer_[i]);
    }
    ImGui::End();
}

void BPMMeasure::DrawWave(Camera* camera) {
    Draw(camera);
}
