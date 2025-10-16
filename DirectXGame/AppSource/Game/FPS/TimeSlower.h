#pragma once
#include <Tools/FPS/FPSObserver.h>
#include <optional>

class TimeSlower {
public:
	TimeSlower(FPSObserver* fps);
	~TimeSlower();

	void Update();

	/// <summary>
	/// スロウモーション開始
	/// </summary>
	/// <param name="time">何秒行うか。何も入れないと(ほぼ)永遠</param>
	void StartSlow(float time = 9999999999.0f);
	/// <summary>
	/// スロウモーションを強制終了させる
	/// </summary>
	void EndSlow(bool calculate);

	float GetDeltaTime() { return deltaTime_; }

private:

	//calculateTime_よりtimer_が短くなったら1.0fにゆっくり戻す
	void CalculateDeltaTime();

	float deltaTime_ = 0.0f;

	float timeRate_ = 1.0f;
	const float slowRate_ = 0.3f;
	const float calculateTime_ = 1.0f;

	float timer_ = 0.0f;

	FPSObserver* fps_;

};
