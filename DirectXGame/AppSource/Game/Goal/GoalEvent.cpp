#include "GoalEvent.h"
#include <Game/Scene/GameScene.h>

GoalEvent::GoalEvent(CameraManager* camera) {
	camera_ = camera;
}

GoalEvent::~GoalEvent() {
}

std::unique_ptr<BaseScene> GoalEvent::Update(float deltatime) {
	
	if (isClear_) {
		
		if (preIsClear_) {
			Initialize();
			preIsClear_ = false;
		}

		ClearUpdate(deltatime);

	}
	return nullptr; // 継続中の場合はnullptrを返す
}

void GoalEvent::Initialize() {
	camera_->SetOffset({ 0.0f, 0.0f, -10.0f });
}

void GoalEvent::ClearUpdate(float deltatime) {
	clearTimer_ += deltatime;

	if (clearTimer_ >= clearWaitTime_) {

		//シーン遷移

	}
}
