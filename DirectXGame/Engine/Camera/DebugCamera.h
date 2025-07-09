#pragma once
#include "../Math/MyMath.h"
#include "Camera.h"
#include "../Render/Render.h"

class DebugCamera {
public:

	void Initialize(Camera* camera = nullptr);

	void Update();

	/// <summary>
	/// カメラの視点を描画
	/// </summary>
	void Draw(Render* render, Camera* camera);

	Camera GetCamera();
	Transform GetTransform();

private:

	Camera camera_;

	//カメラの親座標
	Vector3 center_{};

	//カメラの球面座標系
	Vector3 spherical_{};

	//親からの距離
	float distance_ = -10.0f;

	//最終的なカメラの位置
	Transform transform_{};

	const float speed_ = 0.003f;
};
