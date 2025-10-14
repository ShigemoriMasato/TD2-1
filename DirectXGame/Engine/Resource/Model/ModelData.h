#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <span>
#include <Transform/Transform.h>
#include <Resource/Texture/TextureManager.h>
#include <Math/MyMath.h>
#include <Render/Resource/DrawResource.h>
#include "ModelSkeleton.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

struct ModelMaterial final {
	std::string name;
	int textureHandle = -1;
};

struct Node {
	QuaternionTransform transform{};
	int nodeIndex = -1;
	Matrix4x4 localMatrix = Matrix::MakeIdentity4x4();
	std::string name{};
	std::vector<Node> children{};
};

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr<ID3D12Resource> palleteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> palleteSrvHandle;
};


class ModelData {
public:

	struct VertexResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		std::shared_ptr<D3D12_VERTEX_BUFFER_VIEW> bufferView = nullptr;
		int vertexNum = 0;
	};

	struct IndexResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer = nullptr;
		std::shared_ptr<D3D12_INDEX_BUFFER_VIEW> bufferView = nullptr;
		int indexNum = 0;
	};

	ModelData() = default;
	~ModelData() = default;

	void LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager, DXDevice* device);
	
	Node GetParentNode() { return rootNode_; }
	Skeleton GetSkeleton() { return skeleton_; };
	std::unordered_map<std::string, VertexResource> GetVertexResource() const { return vertexBufferViews_; }
	std::unordered_map<std::string, IndexResource> GetIndexResource() const { return indexBufferViews_; }
	std::vector<ModelMaterial> GetMaterials() const { return material_; }
	int GetNodeCount() const { return nodeCount_; }

protected:

	void LoadMaterial(const aiScene* scene, std::string directoryPath, TextureManager* textureManager);
	Node LoadNode(aiNode* node, const aiScene* scene);
	void CreateSkinCruster(const Skeleton& skeleton);
	void CreateID3D12Resource(ID3D12Device* device);

	std::vector<ModelMaterial> material_{};
	std::unordered_map<std::string, std::vector<ModelVertexData>> vertices_{};
	std::unordered_map<std::string, std::vector<uint32_t>> indices_{};

	std::unordered_map<std::string, VertexResource> vertexBufferViews_{};
	std::unordered_map<std::string, IndexResource> indexBufferViews_{};

	int nodeCount_ = 0;

	Node rootNode_{};
	Skeleton skeleton_{};
	SkinCluster skinCluster_{};
	std::unordered_map<std::string, JointWeightData> skinClusterData;
};

