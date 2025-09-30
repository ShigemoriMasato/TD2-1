#include "ParticleResource.h"
#include <Core/DXCommonFunction.h>
#include <Math/MyMath.h>

DXDevice* ParticleResource::dxDevice_ = nullptr;
SRVManager* ParticleResource::srvManager_ = nullptr;
using namespace Matrix;

ParticleResource::ParticleResource() {
}

ParticleResource::~ParticleResource() {
}

void ParticleResource::Initialize(uint32_t vertexNum, uint32_t instanceNum, uint32_t indexNum) {
	psoConfig_ = PSOConfig{};

	auto device = dxDevice_->GetDevice();

	//リソースの生成とマップ
	vertexResource.Attach(CreateBufferResource(device, sizeof(VertexData) * vertexNum));
	vertexResource->Map(0, nullptr, (void**)&vertex_);
	localPos_.resize(vertexNum);
	texcoord_.resize(vertexNum);
	normal_.resize(vertexNum);

	//頂点のバッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//マテリアル
	materialResource.Attach(CreateBufferResource(device, sizeof(Material)));
	materialResource->Map(0, nullptr, (void**)&material_);
	*material_ = Material{};

	if (indexNum > 0) {
		indexResource.Attach(CreateBufferResource(device, sizeof(uint32_t) * indexNum));
		indexResource->Map(0, nullptr, (void**)&indices_);

		indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = sizeof(uint32_t) * indexNum;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	//Matrix
	matrixResource.Attach(CreateBufferResource(device, sizeof(MatrixData) * instanceNum));
	matrixResource->Map(0, nullptr, reinterpret_cast<void**>(&matrix_));

	for(uint32_t i = 0; i < instanceNum; ++i) {
		matrix_[i].world = MakeIdentity4x4();
		matrix_[i].wvp = MakeIdentity4x4();
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = instanceNum;
	srvDesc.Buffer.StructureByteStride = sizeof(MatrixData);

	D3D12_CPU_DESCRIPTOR_HANDLE matrixCPUHandle = srvManager_->GetCPUHandle();
	matrixGPUHandle_ = srvManager_->GetGPUHandle();

	device->CreateShaderResourceView(matrixResource.Get(), &srvDesc, matrixCPUHandle);

	position_.resize(instanceNum);
	rotate_.resize(instanceNum);
	scale_.resize(instanceNum, { 1.0f, 1.0f, 1.0f });

	vertexNum_ = vertexNum;
	indexNum_ = indexNum;
	instanceNum_ = instanceNum;

	psoConfig_.rootID = RootSignatureID::Particle;
}

void ParticleResource::DrawReady() {
	//InputLayout
	localPos_.resize(vertexNum_);
	texcoord_.resize(vertexNum_);
	normal_.resize(vertexNum_);
	for (uint32_t i = 0; i < vertexNum_; ++i) {
		vertex_[i].position = { localPos_[i].x, localPos_[i].y, localPos_[i].z, 1.0f };
		vertex_[i].texcoord = texcoord_[i];
		vertex_[i].normal = normal_[i];
	}

	//IndexBuffer
	if (indexNum_ > 0) {
		index_.resize(indexNum_);
		std::memcpy(indices_, index_.data(), sizeof(uint32_t) * indexNum_);
	}

	//Material
	material_->color = {
		((color_ >> 24) & 0xff) / 255.0f,
		((color_ >> 16) & 0xff) / 255.0f,
		((color_ >> 8) & 0xff) / 255.0f,
		((color_ >> 0) & 0xff) / 255.0f
	};
	material_->uvTransform = {
		textureScale_.x * cosf(textureRotate_), -textureScale_.y * sinf(textureRotate_), 0.0f, 0.0f,
		textureScale_.x * sinf(textureRotate_), textureScale_.y * cosf(textureRotate_), 0.0f, 0.0f,
		texturePos_.x, texturePos_.y, 1.0f, 0.0f,
	};

	//Matrix
	for (uint32_t i = 0; i < instanceNum_; ++i) {
		matrix_[i].world = MakeScaleMatrix(scale_[i]) * MakeRotationMatrix(rotate_[i]) * MakeTranslationMatrix(position_[i]);
		if (camera_) {
			matrix_[i].wvp = matrix_[i].world * camera_->GetVPMatrix();
		} else {
			matrix_[i].wvp = matrix_[i].world;
		}
	}

	psoConfig_.rootID = RootSignatureID::Particle;
	psoConfig_.vs = "Particle.VS.hlsl";
	psoConfig_.ps = "Particle.PS.hlsl";
}

D3D12_INDEX_BUFFER_VIEW ParticleResource::GetIndexBufferView() const {
	if (!indexResource) {
		return D3D12_INDEX_BUFFER_VIEW{};
	}
	return indexBufferView;
}

ID3D12Resource* ParticleResource::GetMatrixResource() const {
	if (matrixResource) {
		return matrixResource.Get();
	}
	return nullptr;
}
