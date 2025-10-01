#include "TestEmitter.h"

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
}

TestEmitter::~TestEmitter() {
}

void TestEmitter::Initialize(Camera* camera) {
	particle_->psoConfig_.blendID = BlendStateID::Add;
	particle_->psoConfig_.rootID = RootSignatureID::Particle;
	particle_->psoConfig_.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	particle_->camera_ = camera;
}

void TestEmitter::Update() {
	++frame_;

	if (frame_ > coolTime_) {
		frame_ = 0;

		std::uniform_real_distribution dist(-1.0f, 1.0f);

		velocity_[particleNum_] = {
			dist(randEngine_),
			dist(randEngine_),
			dist(randEngine_)
		};
		particle_->position_[particleNum_] = { 0.0f, 0.0f, 0.0f };
		particle_->color_[particleNum_] = 0xffffffff;

		++particleNum_;
	}

	for (int i = 0; i < particleNum_; ++i) {
		++life_[i];
		particle_->color_[i] = 0xffffff00 | static_cast<uint32_t>(0xff * static_cast<float>(120 - life_[i]) / 120.0f);
	}

	particle_->position_.resize(particleNum_);
}

void TestEmitter::Draw(Render* render) {
	render->Draw(particle_.get());
}
