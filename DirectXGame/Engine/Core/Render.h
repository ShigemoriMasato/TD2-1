#pragma once
#include "../../Tools/MyMath.h"
#include "MyDirectX.h"

class Camera;

class Render {
public:
	Render(MyDirectX* myDirectX) : myDirectX_(myDirectX), isCanDraw_(myDirectX_->GetIsCanDraw()) {}
	~Render() = default;

	void DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawSphere(float radius, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawModel(int modelHandle, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData);

	void DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawPrism(Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle);

	void DrawBox(Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle);

private:

	bool IsCanDraw() const {
		return *isCanDraw_ ? *isCanDraw_ : true;
	}

	MyDirectX* myDirectX_;
	bool* isCanDraw_ = nullptr;

};
