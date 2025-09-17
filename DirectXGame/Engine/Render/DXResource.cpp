#include "DXResource.h"
#include <Core/DXCommonFunction.h>

DXDevice* DXResource::dxDevice_ = nullptr;

DXResource::DXResource() {
}

DXResource::~DXResource() {
}

void DXResource::Initialize(uint32_t vertexNum, uint32_t indexNum, bool useMatrix) {
	auto device = dxDevice_->GetDevice();

	//リソースの生成とマップ
	vertexResource = CreateBufferResource(device, sizeof(VertexData) * vertexNum);
	vertexResource->Map(0, nullptr, (void**)&vertex_);

	//頂点のバッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	if (indexNum > 0) {
		indexResource = CreateBufferResource(device, sizeof(uint32_t) * indexNum);
		indexResource->Map(0, nullptr, (void**)&index_);

		indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = sizeof(uint32_t) * indexNum;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	materialResource = CreateBufferResource(device, sizeof(MaterialData));
	materialResource->Map(0, nullptr, (void**)&material_);

	if (useMatrix) {
		matrixResource = CreateBufferResource(device, sizeof(TramsformMatrixData));
		matrixResource->Map(0, nullptr, (void**)&matrix_);
	}

	lightResource = CreateBufferResource(device, sizeof(DirectionalLightData));
	lightResource->Map(0, nullptr, (void**)&light_);

	vertexNum_ = vertexNum;
	indexNum_ = indexNum;
}

D3D12_INDEX_BUFFER_VIEW DXResource::GetIndexBufferView() const {
	if (!indexResource) {
		return D3D12_INDEX_BUFFER_VIEW{};
	}
	return indexBufferView;
}

ID3D12Resource* DXResource::GetMatrixResource() const {
	if(matrixResource) {
		return matrixResource.Get();
	}
	return nullptr;
}
