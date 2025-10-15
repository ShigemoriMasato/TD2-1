#include "ModelResource.h"
#include <Tools/FPS/FPSObserver.h>

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

	Initialize(modelData);
	SetAnimation(manager, modelHandle);
}

void ModelResource::Initialize(ModelData* modelData) {
	node_ = modelData->rootNode_;

	modelData_ = modelData;

	skeleton_ = modelData_->skeleton_;

	matrixResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(MatrixData) * modelData->GetNodeCount()));
	matrixResource_->Map(0, nullptr, (void**)&matrix_);

	materialResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(Material)));
	materialResource_->Map(0, nullptr, (void**)&material_);

	psoConfig_.vs = "Model.VS.hlsl";
	psoConfig_.ps = "Model.PS.hlsl";
	psoConfig_.rootID = RootSignatureID::Model;
	psoConfig_.inputLayoutID = InputLayoutID::Model;
}

void ModelResource::SetAnimation(ModelManager* manager, int modelHandle) {
	auto animation = manager->GetAnimation(modelHandle);
	if (animation.nodeAnimations.size() == 0) {
		return;
	}
	animation_ = std::make_unique<Animation>();
	*animation_ = animation;
	animationTime_ = 0.0f;
}

void ModelResource::DrawReady() {

	material_->color = {
		((color_ >> 16) & 0xff) / 255.0f,
		((color_ >> 8) & 0xff) / 255.0f,
		(color_ & 0xff) / 255.0f,
		((color_ >> 24) & 0xff) / 255.0f
	};
	
	animationTime_ += 1.f / 60;
	animationTime_ = std::fmod(animationTime_, animation_->duration);
	SkeletonAnimation(skeleton_, *animation_, animationTime_);
	SkeletonUpdate(skeleton_);

	modelData_->skinCluster_.Update(skeleton_);

	//todo 変えろ！！！！

	matrix_->world = Matrix::MakeIdentity4x4();
	matrix_->wvp = Matrix::MakeIdentity4x4() * camera_->GetVPMatrix();
}
