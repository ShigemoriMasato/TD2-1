#include "TestEmitter.h"
#include <stdlib.h>

TestEmitter::TestEmitter(int maxNum) : maxNum_(maxNum) {
	particle_ = std::make_unique<ParticleResource>();
	particle_->Initialize(4, 6, maxNum_);
	particle_->localPos_ = {
		{-0.5f, 0.5f, 0.0f},
		{0.5f, 0.5f, 0.0f},
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
	};
	particle_->texcoord_ = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};
	particle_->normal_ = {
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
	};
	particle_->index_ = {
		0, 1, 2,
		1, 3, 2,
	};

	std::random_device rd{};
	randEngine_ = std::mt19937(rd());
	velocity_.resize(maxNum_);
	life_.resize(maxNum_);
	lifeSpan_.resize(maxNum_, lifeTime_);
	particleJobs_.resize(maxNum_);
	std::fill(particle_->color_.begin(), particle_->color_.end(), 0);
	frame_ = 0;
	emitterJobs_ = ParticleJob::ToTransparent | ParticleJob::Scatter;
}

TestEmitter::~TestEmitter() {
}

void TestEmitter::Initialize(Camera* camera) {
	particle_->psoConfig_.blendID = BlendStateID::Add;
	particle_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	particle_->psoConfig_.isSwapChain = true;

	particle_->camera_ = camera;
}

void TestEmitter::Update() {
	++frame_;

	if (frame_ > coolTime_) {
		frame_ = 0;

		for (int i = 0; i < generateNum_; ++i) {

			if (particleNum_ >= maxNum_) {
				particleNum_ = 0;
				SetLifeTime(lifeTime_);
			}

			if (emitterJobs_ & ParticleJob::Scatter) {
				std::uniform_real_distribution dist(-1.0f, 1.0f);

				velocity_[particleNum_] = {
					dist(randEngine_),
					dist(randEngine_),
					dist(randEngine_)
				};
			} else {
				velocity_[particleNum_] = { 0.0f, 0.0f, 0.0f };
			}

			particle_->color_[particleNum_] = ((rand() % 0xfff) << 20) | ((rand() % 0xfff) << 8) | 0xff;
			particle_->position_[particleNum_] = position_;
			life_[particleNum_] = 0;
			lifeSpan_[particleNum_] = static_cast<int>(lifeTime_);
			particleJobs_[particleNum_] = emitterJobs_;

			++particleNum_;
		}
	}

	for (int i = 0; i < maxNum_; ++i) {
		if (particle_->color_[i] == 0) {
			continue;
		}
		++life_[i];
		particle_->color_[i] = particle_->color_[i] & 0xffffff00 | static_cast<uint32_t>(0xff * static_cast<float>(120 - life_[i]) / 120.0f);
		particle_->position_[i] = particle_->position_[i] + velocity_[i] * 0.01f;

		if (life_[i] > lifeSpan_[i]) {
			particle_->color_[i] = 0;
		}
	}
}

void TestEmitter::Draw(Render* render) {
	render->Draw(particle_.get());
}

void TestEmitter::SetLifeTime(int time) {
	lifeTime_ = time;
	std::fill(lifeSpan_.begin() + particleNum_, lifeSpan_.end(), lifeTime_);
}

void TestEmitter::SetJobs(uint16_t jobs) {
	emitterJobs_ = jobs;
}

void TestEmitter::SetPosition(Vector3 pos) {
	position_ = pos;
}

uint32_t operator|(ParticleJob a, ParticleJob b) {
	return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

uint32_t operator|(uint32_t a, ParticleJob b) {
	return a | static_cast<uint32_t>(b);
}

uint32_t operator&(uint32_t a, ParticleJob b) {
	return a & static_cast<uint32_t>(b);
}
