#pragma once
#include "Data/BaseResource.h"
#include <Resource/Model/ModelManager.h>

class MPResource : public BaseResource {
public:

	MPResource();
	~MPResource();

	void Initialize(ModelData* modelData, int instanceNum);
	void Initialize(ModelManager* manager, int modelHandle, int instanceNum);

	void IsBillboard(bool isBillboard) {
		billboard_ = isBillboard;
	}

	/// <summary>
	/// 描画前準備(Render内で呼ばれるため、プログラム時に呼ぶ必要はない)
	/// </summary>
	void DrawReady() override;

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const { return indexBufferView; };

	D3D12_GPU_DESCRIPTOR_HANDLE GetParticleDataGPUHandle() const { return particleDataGPUHandle_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureGPUHandle() const { return textureGPUHandle_; }

	uint32_t GetVertexNum() const { return vertexNum_; }
	uint32_t GetIndexNum() const { return indexNum_; }
	uint32_t GetInstanceNum() const { return instanceNum_; }

public:		//以下描画設定項目 ---==================

	std::vector<uint32_t> index_{};

	std::vector<Vector3> position_{};
	std::vector<Vector3> rotate_{};
	std::vector<Vector3> scale_ = {};

	std::vector<uint32_t> color_{};
	//テクスチャインデックス(0~7)
	std::vector<int> textureIndex_{};
	//読み込む八枚のテクスチャの一番最初のハンドル
	int textureStartIndex_ = 0;

	Camera* camera_ = nullptr;
	bool billboard_ = false;

	//いじる必要なし
	D3D12_GPU_DESCRIPTOR_HANDLE* textureHandles_ = nullptr;

private:

	ModelParticleData* particleData_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> particleDataResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE particleDataGPUHandle_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	uint32_t indexNum_ = 0;
	uint32_t instanceNum_ = 0;

	//これを変える際は、RootSignatureとShaderも変える
	const uint32_t maxTextureNum_ = 8;

};

