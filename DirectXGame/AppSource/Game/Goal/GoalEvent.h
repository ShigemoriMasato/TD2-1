#pragma once
#include "../Camera/CameraManager.h"
#include "../Player/Player.h"

class BaseScene;
class GoalEvent {
public:

	GoalEvent(CameraManager* camera, Player* player);
	~GoalEvent();

	void SetClear(bool isClear) { isClear_ = isClear; }
	std::unique_ptr<BaseScene> Update(float deltatime);

private:

	void Initialize();
	void ClearUpdate(float deltatime);

	CameraManager* camera_ = nullptr;
	Player* player_ = nullptr;

	bool isClear_ = false;
	bool preIsClear_ = false;

private:// クリア時の更新用

	float clearTimer_ = 0.0f;
	const float clearWaitTime_ = 3.0f;
};
