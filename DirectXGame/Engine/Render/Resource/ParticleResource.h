#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include <Transform/Transform.h>
#include <Core/PSO/PSOConfig.h>
#include <Resource/SRVManager.h>
#include <Camera/Camera.h>
#include "Data/BaseResource.h"

/// <summary>
/// CG2で作成した基本的(?)な描画情報
/// </summary>
class ParticleResource : public BaseResource {
public:

	ParticleResource();
	~ParticleResource();

	static void StaticInitialize(DXDevice* device, SRVManager* srvManager) { dxDevice_ = device; srvManager_ = srvManager; }

	void Initialize(uint32_t vertexNum, uint32_t indexNum = 0, uint32_t instanceNum = 1);

	void IsBillboard(bool isBillboard) {
		billboard_ = isBillboard;
	}

	/// <summary>
	/// 描画前準備(Render内で呼ばれるため、プログラム時に呼ぶ必要はない)
	/// </summary>
	void DrawReady();

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	D3D12_GPU_DESCRIPTOR_HANDLE GetMatrixSRVDesc() const { return matrixGPUHandle_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetParticleDataSRVDesc() const { return particleDataGPUHandle_; }

	uint32_t GetVertexNum() const { return vertexNum_; }
	uint32_t GetIndexNum() const { return indexNum_; }
	uint32_t GetInstanceNum() const { return instanceNum_; }

public:		//以下描画設定項目 ---==================

	int textureHandle_ = 0;

	std::vector<uint32_t> index_{};

	std::vector<Vector3> position_{};
	std::vector<Vector3> rotate_{};
	std::vector<Vector3> scale_ = {};

	std::vector<uint32_t> color_{};

	Camera* camera_ = nullptr;

private:

	uint32_t* indices_ = nullptr;
	ParticleData* particleData_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> particleDataResource = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE matrixGPUHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE particleDataGPUHandle_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	static SRVManager* srvManager_;

	uint32_t indexNum_ = 0;
	uint32_t instanceNum_ = 0;


	bool billboard_ = true;
};
