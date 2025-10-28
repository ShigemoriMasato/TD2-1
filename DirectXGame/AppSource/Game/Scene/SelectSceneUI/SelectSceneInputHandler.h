#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <functional>

// 前方宣言
class AudioManager;

/// <summary>
/// セレクトシーンの入力処理
/// キー入力の検知とコールバック実行を担当
/// </summary>
class SelectSceneInputHandler {
public:
	SelectSceneInputHandler();
	~SelectSceneInputHandler();

	/// <summary>
	/// 初期化（AudioManagerを設定してSEを読み込む）
	/// </summary>
	/// <param name="audio">AudioManagerのポインタ</param>
	void Initialize(AudioManager* audio);

	/// <summary>
	/// 入力処理
	/// </summary>
	/// <param name="keys">キー状態</param>
	void ProcessInput(const std::unordered_map<Key, bool>& keys);

	/// <summary>
	/// ステージ選択変更時のコールバックを設定
	/// </summary>
	/// <param name="callback">コールバック関数（direction: -1=左, +1=右）</param>
	void SetOnStageChangeCallback(std::function<void(int direction)> callback) {
		onStageChange_ = callback;
	}

	/// <summary>
	/// ステージ決定時のコールバックを設定
	/// </summary>
	/// <param name="callback">コールバック関数</param>
	void SetOnStageConfirmCallback(std::function<void()> callback) {
		onStageConfirm_ = callback;
	}

	/// <summary>
	/// 矢印テクスチャ更新用のコールバックを設定
	/// </summary>
	/// <param name="callback">コールバック関数（isLeftPressed, isRightPressed）</param>
	void SetOnArrowUpdateCallback(std::function<void(bool, bool)> callback) {
		onArrowUpdate_ = callback;
	}

private:
	// コールバック関数
	std::function<void(int direction)> onStageChange_;
	std::function<void()> onStageConfirm_;
	std::function<void(bool, bool)> onArrowUpdate_;

	// 前回のキー状態
	bool prevKeyLeft_ = false;
	bool prevKeyRight_ = false;

	// AudioManager
	AudioManager* audio_ = nullptr;

	// サウンドハンドル
	int moveSoundHandle_ = -1;    // 移動音
	int confirmSoundHandle_ = -1; // 決定音
};
