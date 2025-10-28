#include "SelectSceneInputHandler.h"

SelectSceneInputHandler::SelectSceneInputHandler() {
}

SelectSceneInputHandler::~SelectSceneInputHandler() {
}

void SelectSceneInputHandler::ProcessInput(const std::unordered_map<Key, bool>& keys) {

	// 左キーが押された瞬間の処理
	if (keys.at(Key::Left) && !prevKeyLeft_) {
		if (onStageChange_) {
			onStageChange_(-1);  // 左方向（-1）
		}
	}

	// 右キーが押された瞬間の処理
	if (keys.at(Key::Right) && !prevKeyRight_) {
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
		if (onStageConfirm_) {
			onStageConfirm_();
		}
	}
}
