#include "PostEffectResource.h"

const int PostEffectResource::postEffectNum = 8;

PostEffectResource::PostEffectResource() {
}

void PostEffectResource::Initialize() {
	auto windowSize = dxDevice_->GetWindowSize();

	vertexNum_ = 3;
	vertexResource.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(VertexData) * vertexNum_));
	vertexResource->Map(0, nullptr, (void**)&vertex_);

	vertex_[0].position = { -1.0f, -3.0f, 0.0f, 1.0f };
	vertex_[0].texcoord = { 0.0f, 2.0f };
	vertex_[0].normal = { 0.0f, 0.0f, -1.0f };

	vertex_[1].position = { -1.0f, 1.0f, 0.0f, 1.0f };
	vertex_[1].texcoord = { 0.0f, 0.0f };
	vertex_[1].normal = { 0.0f, 0.0f, -1.0f };

	vertex_[2].position = { 3.0f, 1.0f, 0.0f, 1.0f };
	vertex_[2].texcoord = { 2.0f, 0.0f };
	vertex_[2].normal = { 0.0f, 0.0f, -1.0f };

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum_;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	infoResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(PostEffectInfo) * postEffectNum));
	infoResource_->Map(0, nullptr, (void**)&postEffectInfo_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; 
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = postEffectNum;
	srvDesc.Buffer.StructureByteStride = sizeof(PostEffectInfo);

	D3D12_CPU_DESCRIPTOR_HANDLE matrixCPUHandle = srvManager_->GetCPUHandle();
	infoGPUHandle_ = srvManager_->GetGPUHandle();

	dxDevice_->GetDevice()->CreateShaderResourceView(infoResource_.Get(), &srvDesc, matrixCPUHandle);
}

void PostEffectResource::DrawReady() {
}
