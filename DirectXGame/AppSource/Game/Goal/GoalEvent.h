#pragma once
#include "../Camera/CameraManager.h"
#include "../Player/Player.h"
#include "../NumberPlate.h"

class BaseScene;
class GoalEvent {
public:

	GoalEvent(CameraManager* camera, Player* player, PostEffectResource* posteffect, TextureManager* textureManager);
	~GoalEvent();

	void SetClear(bool isClear, float time) { isClear_ = isClear; if (clearTime_ == -1) clearTime_ = int(time); }
	std::unique_ptr<BaseScene> Update(float deltatime);
	void Draw(Render* render);
	bool IsChangeScene() const { return changeScene_; }
	bool IsClear() const { return isClear_; }
	bool IsTransitioning() const { return isClear_ && clearTimer_ >= clearWaitTime_; }

private:

	void Initialize();
	void ClearUpdate(float deltatime);

	CameraManager* camera_ = nullptr;
	Player* player_ = nullptr;

	PostEffectResource* postEffect_ = nullptr;

	std::vector<std::unique_ptr<NumberPlate>> scores_;
	std::unique_ptr<DrawResource> plate_ = nullptr;

	bool isClear_ = false;
	bool preIsClear_ = false;

private:// クリア時の更新用

	float clearTimer_ = 0.0f;
	const float clearWaitTime_ = 1.0f;
	int count;
	int finalScore_;
	int clearTime_ = -1;
	bool changeScene_ = false;
};
