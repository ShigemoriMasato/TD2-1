#pragma once
#include "../Camera/CameraManager.h"
#include "../Player/Player.h"

class BaseScene;
class GoalEvent {
public:

	GoalEvent(CameraManager* camera, Player* player, PostEffectResource* posteffect);
	~GoalEvent();

	void SetClear(bool isClear) { isClear_ = isClear; }
	std::unique_ptr<BaseScene> Update(float deltatime);
	bool IsChangeScene() const { return changeScene_; }
	bool IsClear() const { return isClear_; }
	bool IsTransitioning() const { return isClear_ && clearTimer_ >= clearWaitTime_; }

private:

	void Initialize();
	void ClearUpdate(float deltatime);

	CameraManager* camera_ = nullptr;
	Player* player_ = nullptr;

	PostEffectResource* postEffect_ = nullptr;

	bool isClear_ = false;
	bool preIsClear_ = false;

private:// クリア時の更新用

	float clearTimer_ = 0.0f;
	const float clearWaitTime_ = 2.0f;
	bool changeScene_ = false;
};
