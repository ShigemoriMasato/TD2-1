#include "BackGround.h"
#include <Math/MyMath.h>

using namespace MyMath;

BackGround::BackGround() {
	blocks_ = std::make_unique<ParticleResource>();
	blocks_->Initialize(8, 36, width * height, true);

	blocks_->localPos_ = {
			{-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
			{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}
	};
	blocks_->texcoord_ = {
		{0.0f, 0.0f}, {1.0f, 0.0f},
		{0.0f, 1.0f}, {1.0f, 1.0f},
		{1.0f, 0.0f}, {0.0f, 0.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}
	};
	blocks_->normal_ = {
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}
	};
	blocks_->index_ = {
		//前
		2, 1,3,
		2, 0,1,
		//後
		4, 7,5,
		4, 6,7,
		//左
		6, 3,7,
		6, 2,3,
		//右
		1, 4,5,
		1, 0,4,
		//上
		0, 6,4,
		0, 2,6,
		//下
		3, 5,7,
		3, 1, 5
	};

	blocks_->psoConfig_.ps = "Game/BackGroundL.PS.hlsl";
	blocks_->psoConfig_.vs = "Game/BackGround.VS.hlsl";
	blocks_->psoConfig_.rootID = RootSignatureID::LightParticle;
	blocks_->lightData_->color = { 1.0f, 1.0f, 1.0f,1.0f };
	blocks_->lightData_->direction = { 0.79f, 0.45f, 0.41f };
	blocks_->lightData_->intensity = 1.0f;

	postEffect_ = std::make_unique<PostEffectResource>();
	postEffect_->Initialize();
	postEffect_->SetJobs(PostEffectJob::Blur);
	postEffect_->input_ = OffScreenIndex::GameWindow;
	postEffect_->output_ = OffScreenIndex::GameWindow;

	postEffect_->data_.blur.intensity = 1.0f;
	postEffect_->data_.blur.kernelSize = 2.0f;
}

void BackGround::Initialize(Camera* camera) {
	blocks_->camera_ = camera;

	std::fill(blocks_->color_.begin(), blocks_->color_.end(), 0);

	blockMap_.resize(height);
	for (int i = 0; i < height; ++i) {
		blockMap_[i].resize(width);
		for (int j = 0; j < width; ++j) {

			blocks_->position_[i * width + j] = Vector3(
				j * 5.0f - 30.0f,
				i * 5.0f - 20.0f,
				20.0f + rand() % 2 * 8.0f
			);
			blocks_->scale_[i * width + j] = { 5.0f, 5.0f, 10.0f };
			blocks_->color_[i * width + j] = 0xb8dbdeff;
		}
	}
}

void BackGround::Update(float deltaTime) {
	timer += deltaTime;

	if (timer > changeInterval) {
		timer = 0.0f;

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				blockMap_[i][j].first = (rand() % 2);
				blockMap_[i][j].second = blocks_->position_[i * width + j].z - 20.0f;
			}
		}
	}

	float lerpT = std::min(1.0f, timer);

	for (int i = 0; i < width * height; ++i) {
		float targetZ = (float)blockMap_[i / width][i % width].first ? 8.0f : 0.0f;
		blocks_->position_[i].z = lerp(blockMap_[i / width][i % width].second, targetZ, lerpT) + 20.0f;
	}

	ImGui::Begin("BackGround");
	ImGui::DragFloat("Intensity", &postEffect_->data_.blur.intensity, 0.01f);
	ImGui::DragFloat("KernelSize", &postEffect_->data_.blur.kernelSize, 0.01f);
	ImGui::Separator();
	ImGui::DragFloat3("Direction", &blocks_->lightData_->direction.x, 0.01f);
	blocks_->lightData_->direction = blocks_->lightData_->direction.Normalize();
	ImGui::ColorEdit4("Color", &blocks_->lightData_->color.x);
	ImGui::DragFloat("LIntensity", &blocks_->lightData_->intensity, 0.1f, 0.0f);
	ImGui::End();
}

void BackGround::Draw(Render* render) {
	render->Draw(blocks_.get());
	render->Draw(postEffect_.get());
}
