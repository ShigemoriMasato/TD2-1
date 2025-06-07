#pragma once
#include "MyMath.h"

struct Transform;

//初期値を画面サイズ1280*720と仮定して設定

struct PerspectiveFovDesc {
	float fovY = 0.45f;
	float aspectRatio = 1280 / 720;
	float nearClip = 0.1f;
	float farClip = 100.0f;
};

struct OrthographicDesc {
	float left = -640;
	float top = 640;
	float right = -360;
	float bottom = 360;
	float nearClip = 0.0f;
	float farClip = 1000.0f;
};

class Camera {
public:

	Camera() = default;
	~Camera() = default;
	
	void SetPerspectiveFovMatrix(PerspectiveFovDesc desc);
	void SetOrthographicMatrix(OrthographicDesc desc);

	void MakeMatrix();

	void SetTransform(const Transform& transform) {
		this->transform = transform;
	}

	Matrix4x4 GetViewportMatrix() const;

private:

	Matrix4x4 projectionMatrix = MakeIdentity4x4();		//ワールド行列にこれをかけると正射影になる
	Matrix4x4 vpMatrix = MakeIdentity4x4();			//w抜きviewport変換行列

	Transform transform = {};							//カメラ座標
};
