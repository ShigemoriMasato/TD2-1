#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include <Transform/Transform.h>
#include <Core/PSO/PSOConfig.h>

class DXResource {
public:

	DXResource();
	~DXResource();

	static void SetDevice(DXDevice* device) { dxDevice_ = device; }

	void Initialize(uint32_t vertexNum, uint32_t indexNum = 0, bool useMatrix = false);

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
	ID3D12Resource* GetMaterialResource() const { return materialResource.Get(); }
	ID3D12Resource* GetMatrixResource() const;
	ID3D12Resource* GetLightResource() const { return lightResource.Get(); }

	uint32_t GetVertexNum() const { return vertexNum_; }
	uint32_t GetIndexNum() const { return indexNum_; }

	PSOConfig psoConfig_{};

	int textureHandle_ = 0;

	VertexData* vertex_ = nullptr;
	uint32_t* index_ = nullptr;
	MaterialData* material_ = nullptr;
	MatrixData* matrix_ = nullptr;
	DirectionalLightData* light_ = nullptr;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	static DXDevice* dxDevice_;

	uint32_t vertexNum_ = 0;
	uint32_t indexNum_ = 0;

};
