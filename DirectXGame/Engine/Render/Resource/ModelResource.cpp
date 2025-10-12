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

	node_ = modelData->GetParentNode();

	//データのコピー
	auto vertex = modelData->GetVertexResource();
	auto idnex = modelData->GetIndexResource();
	auto materials = modelData->GetMaterials();
	bones_ = modelData->GetBones();

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

	boneResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(Bone)));
	boneResource_->Map(0, nullptr, (void**)&bone_);

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

	// --- ノードのワールド行列を更新 ---
	for (uint32_t i = 0; i < node_.size(); ++i) {
		if (node_[i].parentIndex >= 0) {
			node_[i].worldMatrix = node_[i].localMatrix * node_[node_[i].parentIndex].worldMatrix;
		} else {
			node_[i].worldMatrix = node_[i].localMatrix;
		}

		matrix_[i].local = node_[i].localMatrix;
		matrix_[i].world = node_[i].worldMatrix * worldMat;
		matrix_[i].wvp = matrix_[i].world * camera_->GetVPMatrix();
	}

	// --- ボーンの最終行列を計算 ---
	for (uint32_t i = 0; i < bones_.size(); ++i) {
		Bone& bone = bones_[i];

		// 対応するノードのワールド行列を参照
		if (bone.nodeIndex == -1) continue;

		const Node& node = node_[bone.nodeIndex];
		Matrix4x4 nodeWorld = node.worldMatrix;

		// 親子関係を持つ場合は、親のボーンワールドを継承
		if (bone.parentIndex >= 0) {
			bone.worldMatrix = bone.localMatrix * bones_[bone.parentIndex].worldMatrix;
		} else {
			bone.worldMatrix = bone.localMatrix;
		}

		// ボーン最終行列（スキニング用）
		// nodeWorld: ノード階層におけるワールド
		// offsetMatrix: バインドポーズからボーンローカルへの逆変換
		bone_->boneMatrix[i] = nodeWorld * bone.offsetMatrix;
	}
}