#include "SoundWave.h"
#include <Audioclient.h>
#include <cassert>
#include <functiondiscoverykeys_devpkey.h>
#include <chrono>
#include <fftw3.h>
#include "Engine/Render/Render.h"

namespace {

    void TrimWaveformBuffer(std::vector<float>& buffer, size_t maxSize, size_t incomingSize) {
        if (buffer.size() + incomingSize > maxSize) {
            size_t excess = (buffer.size() + incomingSize) - maxSize;
            buffer.erase(buffer.begin(), buffer.begin() + excess);
        }
    }
}

SoundWave::SoundWave() {
	logger_ = std::make_unique<Logger>("SoundWave");

	//MMDeviceEnumerator作成
	HRESULT hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&enumerator_)
	);
	assert(SUCCEEDED(hr) && "Failed to create MMDeviceEnumerator instance");

	//デフォルトのオーディオエンドポイントを取得
	hr = enumerator_->GetDefaultAudioEndpoint(eRender, eConsole, &device_);
	assert(SUCCEEDED(hr) && "Failed to get default audio endpoint");

    hr = device_->Activate(
        __uuidof(IAudioClient),
        CLSCTX_INPROC_SERVER,
        NULL,
        reinterpret_cast<void**>(audioClient_.GetAddressOf())
    );
    assert(SUCCEEDED(hr) && "Failed to Activate Device");

    // wfx_にオーディオフォーマット情報を取得する
    WAVEFORMATEX* pFormat;
    hr = audioClient_->GetMixFormat(&pFormat);
    assert(SUCCEEDED(hr) && "Failed to get mix format");

    // 取得した生ポインタを unique_ptr で管理する
    wfx_.reset(pFormat);

	//オーディオクライアントを取得
	logger_->Log(ConvertString(std::format(L"Mix Format: wFormatTag = {}\nnChannels = {}\nnSamolesPerSec = {}\nwBitsPerSample = {}\nnBlockAlign = {}\n",
		wfx_->wFormatTag,
		wfx_->nChannels,
		wfx_->nSamplesPerSec,
		wfx_->wBitsPerSample,
		wfx_->nBlockAlign)));

	//オーディオクライアントの初期化
	hr = audioClient_->Initialize(AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_LOOPBACK,
		0,
		0,
		wfx_.get(),
		NULL
	);
	assert(SUCCEEDED(hr) && "Failed to initialize audio client");

    hr = audioClient_->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(captureClient_.GetAddressOf()));
    assert(SUCCEEDED(hr) && "Failed to Get Service");

	//キャプチャの開始
	hr = audioClient_->Start();
	assert(SUCCEEDED(hr) && "Failed to start audio client");

	logger_->Log("SoundWave initialized successfully");
}

SoundWave::~SoundWave() {
	if (audioClient_) {
		audioClient_->Stop();
	}
}

void SoundWave::AudioCapture(Waveform waveform) {
	if (!audioClient_ || !wfx_) {
		logger_->Log("Audio client or wave format is not initialized.");
		return;
	}

    WaveCapture();

	if (waveform == Waveform::Frequency) {
		ConvertFrequency();
	}
}

void SoundWave::Draw(Camera* camera) {

    std::lock_guard<std::mutex> lock(audioWaveformMutex_);

    // x軸方向の拡大率
    const float mixWidth = -0.8f;
    const float maxWidth = 0.8f;
    // Y軸方向の拡大率(絶対値計算なので0~n)
    const float maxHeight = 0.5f;
    const float minHeight = 0.0f;

    // 波形データが空の場合は何もしない
    if (audioWaveform_.empty()) {
        return;
    }

    for (size_t i = 0; i < audioWaveform_.size() - 1; ++i) {
        // 現在のサンプルのX座標
        float nowX = mixWidth + (float)i / (audioWaveform_.size() - 1) * (maxWidth - mixWidth);
        // 次のサンプルのX座標（四角形の右端）
        float nextX = mixWidth + (float)(i + 1) / (audioWaveform_.size() - 1) * (maxWidth - mixWidth);

        // 現在のサンプルのY座標
        float nowY = std::abs(audioWaveform_[i]) * maxHeight;
        // 次のサンプルのY座標
        float nextY = std::abs(audioWaveform_[i + 1]) * maxHeight;

        Vector4 lb, lt, rb, rt;

        // 左下
        lb.x = nowX;
        lb.y = minHeight - nowY;
        lb.z = 0.0f; lb.w = 1.0f;

        // 左上
        lt.x = nowX;
        lt.y = minHeight + nowY;
        lt.z = 0.0f; lt.w = 1.0f;

        // 右下
        rb.x = nextX;
        rb.y = minHeight - nextY;
        rb.z = 0.0f; rb.w = 1.0f;

        // 右上
        rt.x = nextX;
        rt.y = minHeight + nextY;
        rt.z = 0.0f; rt.w = 1.0f;

        // DrawSpriteを呼び出して四角形を描画
        // material以降の引数はデフォルト値が使われるため指定しない
        Render::DrawSprite(lt, rt, lb, rb, Matrix::MakeAffineMatrix(transform_), camera);
    }
}

std::vector<float> SoundWave::GetWaveform() {
    std::lock_guard<std::mutex> lock(audioWaveformMutex_);
    return audioWaveform_;
}

void SoundWave::WaveCapture() {
    HRESULT hr = S_OK;
    UINT32 numFramesAvailable = 0;
    BYTE* pData = nullptr;
    DWORD flags = 0;
    UINT64 devicePosition = 0;
    UINT64 qpcPosition = 0;

    // 利用可能なパケットサイズを取得
    hr = captureClient_->GetNextPacketSize(&numFramesAvailable);
    if (FAILED(hr)) {
        // エラー処理
        return;
    }

    while (numFramesAvailable != 0) {
        // バッファの取得
        hr = captureClient_->GetBuffer(
            &pData,
            &numFramesAvailable,
            &flags,
            &devicePosition,
            &qpcPosition);
        if (FAILED(hr)) {
            // エラー処理
            break;
        }

        if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
            // 無音データの場合、波形は0で埋める
            std::lock_guard<std::mutex> lock(audioWaveformMutex_);
			TrimWaveformBuffer(audioWaveform_, WAVEFORM_DISPLAY_SAMPLES_, numFramesAvailable);
            for (UINT32 i = 0; i < numFramesAvailable; ++i) {
                audioWaveform_.push_back(0.0f); // 0.0f を追加して無音を表現
            }

        } else if (!(flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY)) {
            // 新しいデータを一時的に追加した後、規定のサイズに収まるように先頭から削除
            std::lock_guard<std::mutex> lock(audioWaveformMutex_);
            // 古いデータを削除して、新しいデータが収まるスペースを確保する
            if (audioWaveform_.size() + numFramesAvailable > WAVEFORM_DISPLAY_SAMPLES_) {
                // 新しいデータ追加後の目標サイズを超過する分を削除
                size_t excess = (audioWaveform_.size() + numFramesAvailable) - WAVEFORM_DISPLAY_SAMPLES_;
                audioWaveform_.erase(audioWaveform_.begin(), audioWaveform_.begin() + excess);
            }
            // ここでデータのpush_back
            if (wfx_->wBitsPerSample == 32 && (wfx_->wFormatTag == WAVE_FORMAT_IEEE_FLOAT || (wfx_->wFormatTag == WAVE_FORMAT_EXTENSIBLE && reinterpret_cast<WAVEFORMATEXTENSIBLE*>(wfx_.get())->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))) {
                float* pFloatData = reinterpret_cast<float*>(pData);
                for (UINT32 i = 0; i < numFramesAvailable; ++i) {
                    audioWaveform_.push_back(pFloatData[i * wfx_->nChannels]);
                }
            } else if (wfx_->wBitsPerSample == 16 && wfx_->wFormatTag == WAVE_FORMAT_PCM) {
                short* pShortData = reinterpret_cast<short*>(pData);
                for (UINT32 i = 0; i < numFramesAvailable; ++i) {
                    audioWaveform_.push_back(static_cast<float>(pShortData[i * wfx_->nChannels]) / 32768.0f);
                }
            }
        }

        // バッファの解放
        hr = captureClient_->ReleaseBuffer(numFramesAvailable);
        if (FAILED(hr)) {
            // エラー処理
            break;
        }

        // 次のパケットサイズを再度チェック
        hr = captureClient_->GetNextPacketSize(&numFramesAvailable);
        if (FAILED(hr)) {
            // エラー処理
            break;
        }
    }
}

void SoundWave::ConvertFrequency() {
    const int N = static_cast<int>(audioWaveform_.size());
    if (N == 0) return;

    // 入力をdoubleに変換
    std::vector<double> input(N);
    for (int i = 0; i < N; ++i) {
        input[i] = static_cast<double>(audioWaveform_[i]);
    }

    // FFT出力バッファ
    fftw_complex* output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (N / 2 + 1));
    fftw_plan plan = fftw_plan_dft_r2c_1d(N, input.data(), output, FFTW_ESTIMATE);
    fftw_execute(plan);

    // audioWaveform_ をスペクトルで上書き
    audioWaveform_.resize(N / 2 + 1);
    for (int i = 0; i < N / 2 + 1; ++i) {
        double re = output[i][0];
        double im = output[i][1];
        audioWaveform_[i] = static_cast<float>(std::sqrt(re * re + im * im));
    }

    for (auto& wave : audioWaveform_) {
		wave /= 30.0f;//波が大きすぎるので小さくする
    }

    fftw_destroy_plan(plan);
    fftw_free(output);
}
