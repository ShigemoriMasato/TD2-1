#pragma once
#include "MyMath.h"

struct Transform;

class Camera {
public:

	Camera() = default;
	~Camera() = default;
	
	void SetPerspectiveFovMatrix(PerspectiveFovDesc desc);
	void SetOrthographicMatrix(OrthographicDesc desc);
	void SetViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	void MakeMatrix();

	void SetTransform(const Transform& transform) {
		this->transform = transform;
	}

	Matrix4x4 GetViewportMatrix() const;

private:

	Matrix4x4 projectionMatrix = MakeIdentity4x4();		//ワールド行列にこれをかけると正射影になる
	Matrix4x4 viewportMatrix = MakeIdentity4x4();		//正射影にこれをかけるとビューポートになる
	Matrix4x4 vpvMatrix = MakeIdentity4x4();			//w抜きviewport変換行列

	Transform transform = {};							//カメラ座標

};

struct PerspectiveFovDesc {
	float fovY = 0.45f;
	float aspectRatio = 1280 / 720;
	float nearClip = 0.1f;
	float farClip = 100.0f;
};

struct OrthographicDesc {
	float left;
	float top;
	float right;
	float bottom;
	float nearClip;
	float farClip;
};
