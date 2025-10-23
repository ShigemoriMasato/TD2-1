#pragma once
#include <Camera/DebugCamera.h>

class CameraManager {
public:

	CameraManager();
	~CameraManager() = default;

	/// <summary>
	/// 初期座標指定系
	/// </summary>
	/// <param name="initPos"></param>
	void Initialize(Vector3 initPos);

	/// <summary>
	/// ターゲットに最初から追従してるタイプ
	/// </summary>
	/// <param name="targetPos"></param>
	void Initialize(Vector3* targetPos);
	void Update(float deltaTime);
	void DrawImGui();

	Camera* GetCamera();

	void SetForcusTarget(Vector3* targetPos);
	void SetOffset(const Vector3& offset);
	void SetScale(float ratio);
	void SetDebug(bool debug);

private://関数



private:// 座行計算用変数

	Vector3* targetPos_ = nullptr;
	Vector3 previousTargetPos_{};
	float pcalculate = 1.0f;

	Vector3 targetOffset_{};
	Vector3 previousOffset_{};
	Vector3 offset_{0.0f, 0.0f, -40.0f};
	float ocalculate = 1.0f;

	Vector3 targetScale_{1.0f, 1.0f, 1.0f};
	Vector3 previousScale_{};
	float scalculate = 1.0f;

	//補完を完了させるまでの時間
	float calculateSec_ = 1.0f;

private:// カメラ

	std::unique_ptr<Camera> camera_ = {};
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	//カメラの座標(Debugの場合center座標)
	Transform transform_{};

private:// デバッグ関係

	bool isDebug_ = false;
};
