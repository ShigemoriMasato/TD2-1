#include "PostEffectResource.h"

PostEffectResource::PostEffectResource() {
}

void PostEffectResource::Initialize() {
	auto windowSize = dxDevice_->GetWindowSize();

	vertexNum_ = 3;
	vertexResource.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(VertexData) * vertexNum_));
	vertexResource->Map(0, nullptr, (void**)&vertex_);

	//左上
	vertex_[0].position = { -1.0f, 1.0f, 1.0f, 1.0f };
	vertex_[0].texcoord = { 0.0f, 0.0f };
	vertex_[0].normal = { 0.0f, 0.0f, -1.0f };

	//右上
	vertex_[1].position = { 3.0f, 1.0f, 1.0f, 1.0f };
	vertex_[1].texcoord = { 2.0f, 0.0f };
	vertex_[1].normal = { 0.0f, 0.0f, -1.0f };

	//左下
	vertex_[2].position = { -1.0f, -3.0f, 1.0f, 1.0f };
	vertex_[2].texcoord = { 0.0f, 2.0f };
	vertex_[2].normal = { 0.0f, 0.0f, -1.0f };

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum_;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	infoResource_.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(InfoForGPU)));
	infoResource_->Map(0, nullptr, (void**)&infoForGPU_);

}

void PostEffectResource::SetJobs(PostEffectJob jobs) {
	this->jobs_ = static_cast<uint32_t>(jobs);
}

void PostEffectResource::SetJobs(uint32_t jobs) {
	this->jobs_ = jobs;
	task_ = jobs;
}

void PostEffectResource::DrawReady() {
	if (!task_) {
		psoConfig_.ps = "Simple.PS.hlsl";
	}

	if (task_ & PostEffectJob::Blur) {

		infoForGPU_->slot1.x = data_.blur.intensity;
		infoForGPU_->slot1.y = data_.blur.kernelSize;

		psoConfig_.ps = data_.blur.shaderFile;
		task_ &= ~PostEffectJob::Blur;

	} else if (task_ & PostEffectJob::Grayscale) {

		infoForGPU_->slot1.z = data_.grayscale.intensity;

		psoConfig_.ps = data_.grayscale.shaderFile;
		task_ &= ~PostEffectJob::Grayscale;

	}

	psoConfig_.ps = shaderBasePath_ + psoConfig_.ps;
	psoConfig_.vs = shaderBasePath_ + "PostEffect.VS.hlsl";
	psoConfig_.depthStencilID = DepthStencilID::Transparent;
	psoConfig_.rootID = RootSignatureID::PostEffect;
}

void PostEffectResource::SimpleDrawReady() {
	psoConfig_.ps = shaderBasePath_ + "Simple.PS.hlsl";
	psoConfig_.vs = shaderBasePath_ + "PostEffect.VS.hlsl";
	psoConfig_.depthStencilID = DepthStencilID::Transparent;
	psoConfig_.rootID = RootSignatureID::PostEffect;
}

bool PostEffectResource::IsContinue() const {
	return task_;
}

uint32_t operator|(PostEffectJob a, PostEffectJob b) {
	return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

uint32_t operator|(uint32_t a, PostEffectJob b) {
	return a | static_cast<uint32_t>(b);
}

uint32_t operator&(uint32_t a, PostEffectJob b) {
	return a & static_cast<uint32_t>(b);
}

uint32_t operator~(PostEffectJob a) {
	return ~static_cast<uint32_t>(a);
}
