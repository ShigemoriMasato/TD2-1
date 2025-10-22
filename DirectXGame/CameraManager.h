#pragma once
#include <Camera/DebugCamera.h>

class CameraManager {
public:

	CameraManager();
	~CameraManager() = default;

	void Initialize();
	void Update(float deltaTime);

	void ForcusonTarget(Vector3* targetPos);
	void SetOffset(const Vector3& offset);
	void SetScale(float ratio);
	void SetDebug(bool debug);

private:

	Vector3* targetPos_ = nullptr;
	Vector3 offset_;

	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	float calculateSpeed_ = 1.0f;

	//カメラの座標
	Transform transform_{};
	//position計算用
	float pcalculate = 1.0f;
	//scale計算用
	float scalculate = 1.0f;
};
