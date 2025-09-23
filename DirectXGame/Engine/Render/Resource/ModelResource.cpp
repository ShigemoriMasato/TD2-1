#include "ModelResource.h"

ModelResource::ModelResource() {
}

ModelResource::~ModelResource() {
}

void ModelResource::Initialize(ModelData* modelData) {
	
	auto vertices = modelData->GetVertices();

	for(auto& [name, v] : vertices) {
		auto resource = std::make_unique<DXResource>();
		resource->Initialize(static_cast<uint32_t>(v.size()), 0, true);
		std::memcpy(resource->vertex_, v.data(), sizeof(VertexData) * v.size());
		resource->textureHandle_ = modelData->GetTextureHandle(name);
		resources_.push_back(std::move(resource));
	}

}

void ModelResource::SetMatrix(const Matrix4x4& world, const Matrix4x4& vp) {
	MatrixData matrixData;
	matrixData.world = world;
	matrixData.wvp = vp;

	for (auto& res : resources_) {
		*res->matrix_ = matrixData;
	}
}

void ModelResource::SetMaterial(const Vector4& color, bool enableLighting, const Matrix4x4& uvTransform) {
	MaterialData materialData;
	materialData.color = color;
	materialData.enableLighting = enableLighting ? 1 : 0;
	materialData.uvTransform = uvTransform;

	for (auto& res : resources_) {
		*res->material_ = materialData;
	}
}

void ModelResource::SetLight(const Vector4& color, const Vector3& direction, float intensity) {
	DirectionalLightData lightData;
	lightData.color = color;
	lightData.direction = direction.Normalize();
	lightData.intensity = intensity;

	for (auto& res : resources_) {
		*res->light_ = lightData;
	}
}

std::vector<DXResource*> ModelResource::GetResources() {
	std::vector<DXResource*> res;
	for (auto& r : resources_) {
		res.push_back(r.get());
	}
	return res;
}
