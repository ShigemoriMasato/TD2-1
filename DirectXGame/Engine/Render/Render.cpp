#include "Render.h"
#include "../Camera/Camera.h"

MyDirectX* Render::myDirectX_ = nullptr;
bool* Render::isCanDraw_ = nullptr;

Render::Render(MyDirectX* myDirectX) {
	myDirectX_ = myDirectX;
	isCanDraw_ = myDirectX_->GetIsCanDraw();
}

void Render::DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!isCanDraw_) {
		return;
	}

	myDirectX_->DrawTriangle(left, top, right, worldMatrix, camera->VPMatrix(), material, dLightData, textureHandle);
}

void Render::DrawSphere(float radius , Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!isCanDraw_) {
		return;
	}

	myDirectX_->DrawSphere(radius, worldMatrix, camera->VPMatrix(), material, dLightData, textureHandle);
}

void Render::DrawModel(int modelHandle, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData) {
	if (!isCanDraw_) {
		return;
	}

	myDirectX_->DrawModel(modelHandle, worldMatrix, camera->VPMatrix(), material, dLightData);
}

void Render::DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!isCanDraw_) {
		return;
	}

	myDirectX_->DrawSprite(lt, rt, lb, rb, worldMatrix, camera->VPMatrix(), material, dLightData, textureHandle);
}

void Render::DrawSprite(Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!isCanDraw_) {
		return;
	}

	myDirectX_->DrawSprite({-0.5f, 0.5f, 0.0f, 1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}, {-0.5f, -0.5f, 0.0f, 1.0f}, {0.5f, -0.5f, 0.0f, 1.0f}, worldMatrix, camera->VPMatrix(), material, dLightData, textureHandle);
}

void Render::DrawPrism(Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!isCanDraw_) {
		return;
	}

	myDirectX_->DrawPrism(worldMatrix, camera->VPMatrix(), material, dLightData, textureHandle);
}

void Render::DrawBox(Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!isCanDraw_) return;
	myDirectX_->DrawBox(worldMatrix, camera->VPMatrix(), material, dLightData, textureHandle);
}
