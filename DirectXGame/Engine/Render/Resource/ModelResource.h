#pragma once
#include "DrawResource.h"
#include <Resource/Model/ModelManager.h>

struct ModelDrawData {
	int textureHandle = -1;
	D3D12_VERTEX_BUFFER_VIEW* vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW* indexBufferView = {};
	int indexNum = 0;
};

/// <summary>
/// ModelDataをもとにDXResourceを生成するクラス
/// </summary>
class ModelResource {
public:

	static void StaticInitialize(DXDevice* device, SRVManager* srvManager) {
		dxDevice_ = device;
		srvManager_ = srvManager;
	}

	ModelResource();
	~ModelResource();

	void Initialize(ModelManager* manager, int modelHandle);

	std::unordered_map<std::string, ModelDrawData> GetModelDrawDatas() const { return modelDrawDatas_; }
	ID3D12Resource* GetMaterialResource() const { return materialResource_.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetMatrixSRVDesc() const { return matrixGPUHandle_; }
	ID3D12Resource* GetBoneResource() const { return boneResource_.Get(); }

	/// <summary>
	/// 描画前準備(Render内で呼ばれるため、プログラム時に呼ぶ必要はない)
	/// </summary>
	void DrawReady();

	Vector3 position_{};
	Quaternion rotate_{};
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	uint32_t color_ = 0xffffffff;

	std::vector<Node> node_{};

	Camera* camera_ = nullptr;

	std::vector<Bone> bones_{};

	PSOConfig psoConfig_{};

private:

	void DrawReadyNodeAndBone();

	std::unordered_map<std::string, ModelDrawData> modelDrawDatas_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> boneResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE matrixGPUHandle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE boneGPUHandle_ = {};

	ModelMatrixData* matrix_ = nullptr;
	BoneMatrix* bone_ = nullptr;
	Material* material_ = nullptr;

	static DXDevice* dxDevice_;
	static SRVManager* srvManager_;
};
