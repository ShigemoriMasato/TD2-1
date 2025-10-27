#pragma once
#include "Data/BaseResource.h"
#include <Resource/Model/ModelManager.h>

class ModelParticle : public BaseResource {
public:

	ModelParticle(int instance) : instance_(instance) {};
	~ModelParticle() = default;

	void Initialize(ModelManager* modelManager, int handle);
	void Initialize(ModelManager* modelManager, std::string filePath);
	void Initialize(ModelData* modelData);

	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView; }
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetParticleDataGpuHandle() { return particleDataGpuHandle_; }

	int GetIndexNum() const { return indexNum; }

	void DrawReady() override;

	Transform parent_{};
	std::vector<Transform> transform_{};
	std::vector<uint32_t> color_{};

	Camera* camera_ = nullptr;

	const int instance_;

	int textureHandle = -1;

private:

	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	int indexNum = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> particleResource_ = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE particleDataGpuHandle_{};

	ParticleData* particleData_ = nullptr;
};
