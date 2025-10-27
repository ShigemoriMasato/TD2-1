#include "ModelParticle.h"

using namespace Matrix;

void ModelParticle::Initialize(ModelManager* modelManager, int handle) {
	auto data = modelManager->GetModelData(handle);
	Initialize(data);
}

void ModelParticle::Initialize(ModelManager* modelManager, std::string filePath) {
	int handle = modelManager->LoadModel(filePath);
	Initialize(modelManager, handle);
}

void ModelParticle::Initialize(ModelData* modelData) {
	auto vertex = modelData->GetVertexResource();
	auto index = modelData->GetIndexResource();
	auto materials = modelData->GetMaterials();
	int nodeCount = modelData->GetNodeCount();

	std::string materialName = materials.front().name;

	vertexBufferView = *vertex[materialName].bufferView;
	vertexNum_ = vertex[materialName].vertexNum;
	indexBufferView = *index[materialName].bufferView;
	indexNum = index[materialName].indexNum;

	textureHandle = materials.front().textureHandle;

	particleResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(ParticleData) * instance_));
	particleResource_->Map(0, nullptr, reinterpret_cast<void**>(&particleData_));

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = nodeCount;
		srvDesc.Buffer.StructureByteStride = sizeof(ParticleData);

		D3D12_CPU_DESCRIPTOR_HANDLE matrixCPUHandle = srvManager_->GetCPUHandle();
		particleDataGpuHandle_ = srvManager_->GetGPUHandle();

		dxDevice_->GetDevice()->CreateShaderResourceView(particleResource_.Get(), &srvDesc, matrixCPUHandle);
	}

	transform_.resize(instance_);
	color_.resize(instance_);
}

void ModelParticle::DrawReady() {
	Matrix4x4 local = MakeAffineMatrix(parent_);

	for (int i = 0; i < instance_; ++i) {
		particleData_[i].world = local * MakeAffineMatrix(transform_[i]);
		particleData_[i].wvp = particleData_[i].world * camera_->GetVPMatrix();
		particleData_[i].color = {
			((color_[i] >> 24) & 0xFF) / 255.0f,
			((color_[i] >> 16) & 0xFF) / 255.0f,
			((color_[i] >> 8) & 0xFF) / 255.0f,
			((color_[i]) & 0xFF) / 255.0f
		};
	}
}
