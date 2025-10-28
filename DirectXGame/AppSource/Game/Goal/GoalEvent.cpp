#include "GoalEvent.h"
#include <Game/Scene/GameScene.h>

GoalEvent::GoalEvent(CameraManager* camera, Player* player, PostEffectResource* posteffect) {
	camera_ = camera;
	player_ = player;
	postEffect_ = posteffect;
}

GoalEvent::~GoalEvent() {
}

std::unique_ptr<BaseScene> GoalEvent::Update(float deltatime) {
	
	if (isClear_) {
		
		if (!preIsClear_) {
			Initialize();
			preIsClear_ = true;
		}

		ClearUpdate(deltatime);

	}
	return nullptr; // 継続中の場合はnullptrを返す
}

void GoalEvent::Initialize() {
	camera_->SetOffset({ 0.0f, 0.0f, -10.0f });
	player_->SetIsClear();

	// グリッド遷移の初期設定
	postEffect_->data_.gridTransition.progress = 0.0f;
	postEffect_->data_.gridTransition.gridSize = 16.0f;
	postEffect_->data_.gridTransition.fadeColor = 0.0f;  // 黒にフェード
	postEffect_->data_.gridTransition.pattern = 1.0f;    // ランダムパターン

	// 最初はエフェクトなし（クリア演出中）
	postEffect_->SetJobs(PostEffectJob::None);
}

void GoalEvent::ClearUpdate(float deltatime) {
	clearTimer_ += deltatime;

	if (clearTimer_ >= clearWaitTime_) {
		// クリア演出終了後、グリッド遷移を開始
		postEffect_->SetJobs(PostEffectJob::GridTransition);
		
		// グリッド遷移を進める
		postEffect_->data_.gridTransition.progress += deltatime / 2.0f;
		postEffect_->data_.gridTransition.progress = std::min(postEffect_->data_.gridTransition.progress, 1.0f);
	}

	if(postEffect_->data_.gridTransition.progress >= 1.0f){
		changeScene_ = true;
	}
}
