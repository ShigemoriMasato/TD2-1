#include "GoalEvent.h"
#include <Game/Scene/GameScene.h>

GoalEvent::GoalEvent(CameraManager* camera, Player* player) {
	camera_ = camera;
	player_ = player;
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
}

void GoalEvent::ClearUpdate(float deltatime) {
	clearTimer_ += deltatime;

	if (clearTimer_ >= clearWaitTime_) {
		//シーン遷移
		//ポストエフェクトをかけてセレクトsceneに移行。UIはめんどくさいから作らない
	}
}
