#include "ModelResource.h"

DXDevice* ModelResource::dxDevice_ = nullptr;
SRVManager* ModelResource::srvManager_ = nullptr;

ModelResource::ModelResource() {
}

ModelResource::~ModelResource() {
}

void ModelResource::Initialize(ModelManager* manager, int modelHandle) {
	auto modelData = manager->GetModelData(modelHandle);
	if (!modelData) {
		assert(false && "ModelData is nullptr");
		return;
	}

	//データのコピー
	auto vertex = modelData->GetVertexResource();
	auto idnex = modelData->GetIndexResource();
	auto materials = modelData->GetMaterials();

	node_ = modelData->GetParentNode();
	skeleton_ = modelData->GetSkeleton();


	for (const auto& material : materials) {
		ModelDrawData drawData{};
		drawData.textureHandle = material.textureHandle;

		drawData.vertexBufferView = vertex[material.name].bufferView.get();
		drawData.indexBufferView = idnex[material.name].bufferView.get();
		drawData.indexNum = idnex[material.name].indexNum;

		modelDrawDatas_[material.name] = drawData;
	}

	matrixResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(ModelMatrixData) * modelData->GetNodeCount()));
	matrixResource_->Map(0, nullptr, (void**)&matrix_);
	
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = modelData->GetNodeCount();
		srvDesc.Buffer.StructureByteStride = sizeof(MatrixData);

		D3D12_CPU_DESCRIPTOR_HANDLE matrixCPUHandle = srvManager_->GetCPUHandle();
		matrixGPUHandle_ = srvManager_->GetGPUHandle();

		dxDevice_->GetDevice()->CreateShaderResourceView(matrixResource_.Get(), &srvDesc, matrixCPUHandle);
	}

	materialResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(Material)));
	materialResource_->Map(0, nullptr, (void**)&material_);

	psoConfig_.vs = "Model.VS.hlsl";
	psoConfig_.ps = "Model.PS.hlsl";
	psoConfig_.rootID = RootSignatureID::Model;
	psoConfig_.inputLayoutID = InputLayoutID::Model;
}

void ModelResource::DrawReady() {

	material_->color = {
		((color_ >> 16) & 0xff) / 255.0f,
		((color_ >> 8) & 0xff) / 255.0f,
		(color_ & 0xff) / 255.0f,
		((color_ >> 24) & 0xff) / 255.0f
	};

	DrawReadyNodeAndBone();
}

void ModelResource::DrawReadyNodeAndBone() {
	Matrix4x4 worldMat = Matrix::MakeScaleMatrix(scale_) * rotate_.ToMatrix() * Matrix::MakeTranslationMatrix(position_);

	int index = 0;
	for (Joint& joint : skeleton_.joints) {
		joint.localMatrix = Matrix::MakeScaleMatrix(joint.scale) *
			joint.rotation.ToMatrix() *
			Matrix::MakeTranslationMatrix(joint.translation);

		if (joint.parent) {
			joint.skeltonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeltonSpaceMatrix;
		} else {
			joint.skeltonSpaceMatrix = joint.localMatrix;
		}

		matrix_[index].world = joint.skeltonSpaceMatrix * worldMat;
	}
}