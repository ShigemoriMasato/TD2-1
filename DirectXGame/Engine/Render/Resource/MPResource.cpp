#include "MPResource.h"

MPResource::MPResource() {
}

MPResource::~MPResource() {
}

void MPResource::Initialize(ModelData* modelData, int instanceNum) {
	psoConfig_ = PSOConfig{};

	int vertexNum = modelData->GetVertexResource().begin()->second.vertexNum;
	int indexNum = modelData->GetIndexResource().begin()->second.indexNum;

	auto device = dxDevice_->GetDevice();

	vertexBufferView = *modelData->GetVertexResource().begin()->second.bufferView;
	indexBufferView = *modelData->GetIndexResource().begin()->second.bufferView;

	//particleData
	particleDataResource.Attach(CreateBufferResource(device, sizeof(ModelParticleData) * instanceNum));
	particleDataResource->Map(0, nullptr, reinterpret_cast<void**>(&particleData_));

	for (int i = 0; i < instanceNum; ++i) {
		particleData_[i].world = MakeIdentity4x4();
		particleData_[i].wvp = MakeIdentity4x4();
		particleData_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		particleData_[i].textureIndex = 0;
	}

	color_.resize(instanceNum, 0xffffffff);
	textureIndex_.resize(instanceNum, 0);

	//ParticleDataのSRV作成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = instanceNum;
		srvDesc.Buffer.StructureByteStride = sizeof(ParticleData);

		D3D12_CPU_DESCRIPTOR_HANDLE matrixCPUHandle = srvManager_->GetCPUHandle();
		particleDataGPUHandle_ = srvManager_->GetGPUHandle();

		device->CreateShaderResourceView(particleDataResource.Get(), &srvDesc, matrixCPUHandle);
	}

	position_.resize(instanceNum);
	rotate_.resize(instanceNum);
	scale_.resize(instanceNum, { 1.0f, 1.0f, 1.0f });

	vertexNum_ = vertexNum;
	indexNum_ = indexNum;
	instanceNum_ = instanceNum;
}

void MPResource::Initialize(ModelManager* manager, int modelHandle, int instanceNum) {
	auto modelData = manager->GetModelData(modelHandle);
	if (!modelData) {
		return;
	}
	Initialize(modelData, instanceNum);
}

void MPResource::DrawReady() {
	//particleData
	Matrix4x4 bill = MakeIdentity4x4();
	if (billboard_) {
		// ビルボード
		bill = Inverse(camera_->GetTranformMatrix());
		for (int i = 0; i < 3; ++i) {
			bill.m[3][i] = 0.0f;
		}
	}

	//各行列の作成
	for (uint32_t i = 0; i < instanceNum_; ++i) {
		particleData_[i].world = MakeScaleMatrix(scale_[i]) * MakeRotationMatrix(rotate_[i]) * bill * MakeTranslationMatrix(position_[i]);

		if (camera_) {
			particleData_[i].wvp = particleData_[i].world * camera_->GetVPMatrix();
		}

		particleData_[i].color = {
			((color_[i] >> 24) & 0xff) / 255.0f,
			((color_[i] >> 16) & 0xff) / 255.0f,
			((color_[i] >> 8) & 0xff) / 255.0f,
			(color_[i] & 0xff) / 255.0f
		};

		particleData_[i].textureIndex = textureIndex_[i];
	}

	psoConfig_.ps = "MP.PS.hlsl";
	psoConfig_.vs = "MP.VS.hlsl";
	psoConfig_.rootID = RootSignatureID::MP;
	psoConfig_.inputLayoutID = InputLayoutID::Default;
}
