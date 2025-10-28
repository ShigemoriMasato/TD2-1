#include "SelectSceneInputHandler.h"
#include <Resource/Audio/AudioManager.h>

SelectSceneInputHandler::SelectSceneInputHandler() {
}

SelectSceneInputHandler::~SelectSceneInputHandler() {
}

void SelectSceneInputHandler::Initialize(AudioManager* audio) {
	audio_ = audio;

	// SEの読み込み
	if (audio_) {
		moveSoundHandle_ = audio_->Load("select.mp3");    // 移動音
		confirmSoundHandle_ = audio_->Load("dicision.mp3"); // 決定音
	}
	audio->SetVolume(confirmSoundHandle_, 1.5f);
}

void SelectSceneInputHandler::ProcessInput(const std::unordered_map<Key, bool>& keys) {

	// 左キーが押された瞬間の処理
	if (keys.at(Key::Left) && !prevKeyLeft_) {
		// 移動SEを再生
		if (audio_ && moveSoundHandle_ != -1) {
			audio_->Play(moveSoundHandle_, false);
		}

		if (onStageChange_) {
			onStageChange_(-1);  // 左方向（-1）
		}
	}

	// 右キーが押された瞬間の処理
	if (keys.at(Key::Right) && !prevKeyRight_) {
		// 移動SEを再生
		if (audio_ && moveSoundHandle_ != -1) {
			audio_->Play(moveSoundHandle_, false);
		}

		if (onStageChange_) {
			onStageChange_(1);  // 右方向（+1）
		}
	}

	// 矢印テクスチャの更新
	if (onArrowUpdate_) {
		onArrowUpdate_(keys.at(Key::Left), keys.at(Key::Right));
	}

	// キー状態を保存
	prevKeyLeft_ = keys.at(Key::Left);
	prevKeyRight_ = keys.at(Key::Right);

	// Spaceキーでステージ決定
	if (keys.at(Key::Action)) {
		// 決定SEを再生
		if (audio_ && confirmSoundHandle_ != -1) {
			audio_->Play(confirmSoundHandle_, false);
		}

		if (onStageConfirm_) {
			onStageConfirm_();
		}
	}
}
