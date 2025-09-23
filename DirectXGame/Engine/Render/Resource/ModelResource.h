#pragma once
#include "DXResource.h"
#include <Resource/Model/ModelData.h>

class ModelResource {
public:

	ModelResource();
	~ModelResource();

	void Initialize(ModelData* modelData);

	void SetMatrix(const Matrix4x4& world, const Matrix4x4& vp);
	void SetMaterial(const Vector4& color, bool enableLighting, const Matrix4x4& uvTransform);
	void SetLight(const Vector4& color, const Vector3& direction, float intensity);

private:

	std::vector<std::unique_ptr<DXResource>> resources_;

};
