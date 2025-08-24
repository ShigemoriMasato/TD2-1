#pragma once
#include <vector>
#include <unordered_map>
#include "DrawConfig.h"

struct DrawData {
	std::list<VertexData> vertex;
	std::list<uint32_t> index;
};

class DrawCommandManager {
public:

	DrawCommandManager();
	~DrawCommandManager();

	void DrawTriangle(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 worldMatirx, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle);

	void DrawSphere(float radius, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle);

	void DrawModel(int modelHandle, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material);

	void DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle, bool isOffScreen = false);

	void DrawPrism(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle);

	void DrawBox(Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle);

	void DrawLine(Vector4 start, Vector4 end, Matrix4x4 worldMatrix, Matrix4x4 wvpMatrix, MaterialData material, int textureHandle);

	std::vector<std::unordered_map<DrawConfig, DrawData>> pushQueue();

private:

	void MakeDrawData(int drawType, DrawData drawData, MaterialData material, PSOConfig pso, TramsformMatrixData data, int textureID);
	
	//DrawType->DrawConfig->VertexList
	std::vector<std::unordered_map<DrawConfig, DrawData>> drawQueue_;

};
