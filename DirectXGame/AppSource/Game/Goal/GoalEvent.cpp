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

	postEffect_->SetJobs(PostEffectJob::GridTransition | PostEffectJob::Fade);
}

void GoalEvent::ClearUpdate(float deltatime) {
	clearTimer_ += deltatime;

	if (clearTimer_ >= clearWaitTime_) {
		//シーン遷移
		postEffect_->data_.gridTransition.progress += std::min(deltatime / 2.0f, 1.0f);
	}

	if(postEffect_->data_.gridTransition.progress >= 1.0f){
		changeScene_ = true;
	}
}
