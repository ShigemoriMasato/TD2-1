#pragma once
#include "../Camera/CameraManager.h"

class BaseScene;
class GoalEvent {
public:

	GoalEvent(CameraManager* camera);
	~GoalEvent();

	void SetClear(bool isClear) { isClear_ = isClear; }
	std::unique_ptr<BaseScene> Update(float deltatime);

private:

	void Initialize();
	void ClearUpdate(float deltatime);

	CameraManager* camera_ = nullptr;

	bool isClear_ = false;
	bool preIsClear_ = false;

private:// クリア時の更新用

	float clearTimer_ = 0.0f;
	const float clearWaitTime_ = 3.0f;
};
