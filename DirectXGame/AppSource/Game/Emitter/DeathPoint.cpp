#include "DeathPoint.h"

void DeathPoint::Initialize(std::vector<Vector3> deathPoints, int textureHandle, Camera* camera) {
	int instance = uint32_t(deathPoints.size());
	if (instance == 0) return;

	res_ = std::make_unique<ParticleResource>();
	res_->Initialize(4, 6, instance);
	size_ = 0.5f;
	res_->localPos_ = {
		{-size_, size_ * 2.0f, 0.0f},
		{size_, size_ * 2.0f, 0.0f},
		{-size_, 0.0f, 0.0f},
		{size_, 0.0f, 0.0f}
	};
	res_->texcoord_ = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f}
	};
	res_->normal_ = {
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}
	};
	res_->index_ = {
		0, 1, 2,
		2, 1, 3
	};

	for(int i = 0; i < instance; ++i) {
		res_->position_[i] = deathPoints[i];
		res_->position_[i].z = -0.6f;
		res_->color_[i] = 0xffffffff;
	}

	res_->textureHandle_ = textureHandle;

	res_->camera_ = camera;
}

void DeathPoint::Draw(Render* render) {
	if (res_) {

		res_->localPos_ = {
				{-size_, size_, 0.0f},
				{size_, size_, 0.0f},
				{-size_, -size_, 0.0f},
				{size_, -size_, 0.0f}
		};

		ImGui::Begin("res");
		ImGui::DragFloat("Size", &size_, 0.01f);
		ImGui::End();

		render->Draw(res_.get());
	}
}
