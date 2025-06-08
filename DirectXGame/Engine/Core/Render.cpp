#include "Render.h"
#include "../../Tools/Camera.h"

void Render::DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!IsCanDraw()) {
		return;
	}

	myDirectX_->DrawTriangle(left, top, right, worldMatrix, camera->GetViewportMatrix(), material, dLightData, textureHandle);
}

void Render::DrawSphere(float radius , Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!IsCanDraw()) {
		return;
	}

	myDirectX_->DrawSphere(radius, worldMatrix, camera->GetViewportMatrix(), material, dLightData, textureHandle);
}

void Render::DrawModel(int modelHandle, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData) {
	if (!IsCanDraw()) {
		return;
	}

	myDirectX_->DrawModel(modelHandle, worldMatrix, camera->GetViewportMatrix(), material, dLightData);
}

void Render::DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!IsCanDraw()) {
		return;
	}

	myDirectX_->DrawSprite(lt, rt, lb, rb, worldMatrix, camera->GetViewportMatrix(), material, dLightData, textureHandle);
}

void Render::DrawPrism(Matrix4x4 worldMatrix, Camera* camera, MaterialData material, DirectionalLightData dLightData, int textureHandle) {
	if (!IsCanDraw()) {
		return;
	}

	myDirectX_->DrawPrism(worldMatrix, camera->GetViewportMatrix(), material, dLightData, textureHandle);
}
