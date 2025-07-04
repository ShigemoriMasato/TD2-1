#pragma once
#include "../Math/MyMath.h"
#include "MyDirectX.h"

class Camera;

class Render {
public:
	Render(MyDirectX* myDirectX);
	~Render() = default;

	static void DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatrix, Camera* camera, MaterialData material = {1.0f, 1.0f, 1.0f, 1.0f, true}, DirectionalLightData dLightData = {}, int textureHandle = 1);

	static void DrawSphere(float radius, Matrix4x4 worldMatrix, Camera* camera, MaterialData material = {1.0f, 1.0f, 1.0f, 1.0f, true}, DirectionalLightData dLightData = {}, int textureHandle = 1);

	static void DrawModel(int modelHandle, Matrix4x4 worldMatrix, Camera* camera, MaterialData material = {1.0f, 1.0f, 1.0f, 1.0f, true}, DirectionalLightData dLightData = {});

	static void DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldMatrix, Camera* camera, MaterialData material = {1.0f, 1.0f, 1.0f, 1.0f, true}, DirectionalLightData dLightData = {}, int textureHandle = 1);

	static void DrawSprite(Matrix4x4 worldMatrix, Camera* camera, MaterialData material = { 1.0f, 1.0f, 1.0f, 1.0f, true }, DirectionalLightData dLightData = {}, int textureHandle = 1);

	static void DrawPrism(Matrix4x4 worldMatrix, Camera* camera, MaterialData material = {1.0f, 1.0f, 1.0f, 1.0f, true}, DirectionalLightData dLightData = {}, int textureHandle = 1);

	static void DrawBox(Matrix4x4 worldMatrix, Camera* camera, MaterialData material = {1.0f, 1.0f, 1.0f, 1.0f, true}, DirectionalLightData dLightData = {}, int textureHandle = 1);

	static void DrawLine(Vector4 start, Vector4 end, Matrix4x4 worldMatrix, Camera* camera, MaterialData material = { 1.0f, 1.0f, 1.0f, 1.0f, true }, DirectionalLightData dLightData = {}, int textureHandle = 1);

private:

	static MyDirectX* myDirectX_;
	static bool* isCanDraw_;

};
