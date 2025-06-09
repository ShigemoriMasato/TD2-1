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

private:

	Camera camera_;

	//カメラの親座標
	Vector3 center_;

	//親からの距離
	float distance_ = -10.0f;

	//最終的なカメラの位置
	Transform transform_;

	const float speed_ = 0.01f;
};
