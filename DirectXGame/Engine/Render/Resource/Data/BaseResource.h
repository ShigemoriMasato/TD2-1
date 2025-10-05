#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include <Transform/Transform.h>
#include <vector>
#include <Core/PSO/PSOConfig.h>

class BaseResource {
public:

	static void StaticInitialize(DXDevice* device) { dxDevice_ = device; }

	BaseResource() = default;
	virtual ~BaseResource() = default;

	std::vector<Vector3> localPos_{};
	std::vector<Vector3> normal_{};
	std::vector<Vector2> texcoord_{};

	PSOConfig psoConfig_;

protected:

	VertexData* vertex_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	uint32_t vertexNum_ = 0;

	static DXDevice* dxDevice_;

};
