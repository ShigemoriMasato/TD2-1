#include "ModelResource.h"

ModelResource::ModelResource() {
}

ModelResource::~ModelResource() {
}

void ModelResource::Initialize(ModelData* modelData) {
	resources_.clear();

	//ノードの保存
	node_ = modelData->GetParentNode();

	//ノードのリソースを作成
	node_ = CreateResource(node_);
}

void ModelResource::DrawReady() {
	node_ = DrawReadyNode(node_, psoConfig_);
}

Node ModelResource::CreateResource(Node node) {
	//ノードを元としてリソースをマテリアルの数だけ作成
	for (const auto& [materialName, vertexData] : node.vertices) {
		auto resource = std::make_unique<DrawResource>();
		resource->Initialize(static_cast<uint32_t>(vertexData.size()), 0, true);

		for (int i = 0; i < vertexData.size(); ++i) {

			//頂点情報の入力
			resource->localPos_[i] = { vertexData[i].position.x, vertexData[i].position.y, vertexData[i].position.z };
			resource->texcoord_[i] = { vertexData[i].texcoord.x, vertexData[i].texcoord.y };
			resource->normal_[i] = { vertexData[i].normal.x, vertexData[i].normal.y, vertexData[i].normal.z };

		}

		resources_.push_back(std::move(resource));
		node.drawResource[materialName] = resources_.back().get();
	}

	//子ノードに対しても同様の処理
	for(int i = 0; i < node.children.size(); ++i) {
		node.children[i] = CreateResource(node.children[i]);
	}

	return node;
}

Node ModelResource::DrawReadyNode(Node node, PSOConfig config) {
	for (const auto& [materialName, resource] : node.drawResource) {
		resource->psoConfig_ = config;
		resource->textureHandle_ = textureMap_[materialName];
		resource->AddParentMatrix(node.localMatrix);
	}

	for(auto& child : node.children) {
		child = DrawReadyNode(child, config);
	}

	return node;
}

std::vector<DrawResource*> ModelResource::GetResources() {
	std::vector<DrawResource*> res;
	for (const auto& resource : resources_) {
		res.push_back(resource.get());
	}
	return res;
}

void ModelResource::SetMaterial(const uint32_t& color, const int textureHandle, const Vector2 texturePos, const float textureRotate) {
}

void ModelResource::SetLight(bool enableLighting, const uint32_t& color, const Vector3& direction, float intensity) {
}

void ModelResource::SetMatrixData(Vector3 scale, Vector3 rotate, Vector3 pos) {

}

void ModelResource::SetMatrixData(Matrix4x4 world) {
}

void ModelResource::SetCamera(Camera* camera) {
	for(auto& resource : resources_) {
		resource->camera_ = camera;
	}
}
