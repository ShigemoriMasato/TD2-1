#pragma once
#include "DXResource.h"
#include <Resource/Model/ModelData.h>

class ModelResource {
public:

	ModelResource();
	~ModelResource();

	void Initialize(ModelData* modelData);

	/// <summary>
	/// 描画前準備(Render内で呼ばれるため、プログラム時に呼ぶ必要はない)
	/// </summary>
	void DrawReady();

	std::vector<DXResource*> GetResources();

	void SetMatrixData(Vector3 scale, Vector3 rotate, Vector3 pos);
	void SetCamera(Camera* camera);
	void SetMaterial(const uint32_t& color = 0xffffffff, const int textureHandle = 0, const Vector2 texturePos = { 0,0 }, const float textureRotate = 0.0f);
	void SetLight(bool enableLighting = false, const uint32_t& color = 0xffffffff, const Vector3& direction = {0.0f, 0.0f, 1.0f}, float intensity = 0.0f);

	PSOConfig psoConfig_{};

private:

	std::vector<std::unique_ptr<DXResource>> resources_;

};
