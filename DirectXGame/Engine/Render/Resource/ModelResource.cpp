#include "ModelResource.h"

ModelResource::ModelResource() {
}

ModelResource::~ModelResource() {
}

void ModelResource::Initialize(ModelData* modelData) {
	
	auto vertices = modelData->GetVertices();

	for(auto& [name, vertex] : vertices) {
		auto resource = std::make_unique<DrawResource>();
		resource->Initialize(static_cast<uint32_t>(vertex.size()), 0, true);

		for (int i = 0; i < vertex.size(); ++i) {
			resource->localPos_[i] = { vertex[i].position.x, vertex[i].position.y, vertex[i].position.z};
			resource->texcoord_[i] = { vertex[i].texcoord.x, vertex[i].texcoord.y };
			resource->normal_[i] = { vertex[i].normal.x, vertex[i].normal.y, vertex[i].normal.z };
		}

		resource->textureHandle_ = modelData->GetTextureHandle(name);
		resources_.push_back(std::move(resource));
	}

	if (dynamic_cast<SkinningModelData*>(modelData)) {
		node_ = dynamic_cast<SkinningModelData*>(modelData)->GetNode();
	}

	psoConfig_ = resources_.back()->psoConfig_;
}

void ModelResource::DrawReady() {
	for (auto& res : resources_) {
		res->psoConfig_ = psoConfig_;
	}

	if (node_.name != "") {
		for(auto& res : resources_) {
			res->AddParentMatrix(node_.localMatrix);
		}
	}
}

void ModelResource::SetMaterial(const uint32_t& color, const int textureHandle, const Vector2 texturePos, const float textureRotate) {
	for (auto& res : resources_) {
		res->color_ = color;
		res->texturePos_ = texturePos;
		res->textureRotate_ = textureRotate;
		res->textureHandle_ = textureHandle;
	}
}

void ModelResource::SetLight(bool enableLighting, const uint32_t& color, const Vector3& direction, float intensity) {
	for (auto& res : resources_) {
		res->enableLighting_ = enableLighting;
		res->lightColor_ = color;
		res->lightDirection_ = direction;
		res->lightIntensity_ = intensity;
	}
}

std::vector<DrawResource*> ModelResource::GetResources() {
	std::vector<DrawResource*> res;
	for (auto& r : resources_) {
		res.push_back(r.get());
	}
	return res;
}

void ModelResource::SetMatrixData(Vector3 scale, Vector3 rotate, Vector3 pos) {
	for (auto& res : resources_) {
		res->scale_ = scale;
		res->rotate_ = rotate;
		res->position_ = pos;
	}
}

void ModelResource::SetMatrixData(Matrix4x4 world) {
	for (auto& res : resources_) {
		res->SetWorldMatrix(world);
	}
}

void ModelResource::SetCamera(Camera* camera) {
	for (auto& res : resources_) {
		res->camera_ = camera;
	}
}
