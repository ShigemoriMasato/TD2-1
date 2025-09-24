#include "DXResource.h"
#include <Core/DXCommonFunction.h>

DXDevice* DXResource::dxDevice_ = nullptr;

DXResource::DXResource() {
}

DXResource::~DXResource() {
}

void DXResource::Initialize(uint32_t vertexNum, uint32_t indexNum, bool useMatrix) {
	psoConfig_ = PSOConfig{};

	auto device = dxDevice_->GetDevice();

	//リソースの生成とマップ
	vertexResource.Attach(CreateBufferResource(device, sizeof(VertexData) * vertexNum));
	vertexResource->Map(0, nullptr, (void**)&vertex_);

	//頂点のバッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	
	//マテリアル
	materialResource.Attach(CreateBufferResource(device, sizeof(MaterialData)));
	materialResource->Map(0, nullptr, (void**)&material_);
	
	lightResource.Attach(CreateBufferResource(device, sizeof(DirectionalLightData)));
	lightResource->Map(0, nullptr, (void**)&light_);

	if (indexNum > 0) {
		indexResource.Attach(CreateBufferResource(device, sizeof(uint32_t) * indexNum));
		indexResource->Map(0, nullptr, (void**)&index_);

		indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = sizeof(uint32_t) * indexNum;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	if (useMatrix) {
		matrixResource.Attach(CreateBufferResource(device, sizeof(MatrixData)));
		matrixResource->Map(0, nullptr, (void**)&matrix_);
	}

	vertexNum_ = vertexNum;
	indexNum_ = indexNum;

	if (!useMatrix) {
		psoConfig_.rootID = RootSignatureID::NonMatrix;
		psoConfig_.vs = "NonMatrix3d.VS.hlsl";
	}
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
