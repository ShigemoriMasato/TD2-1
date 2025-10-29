#include "DeathParticle.h"

DeathParticle::DeathParticle(Vector3* playerPos) {
	playerPos_ = playerPos;
	res_ = std::make_unique<ParticleResource>();
	res_->Initialize(8, 36, instanceNum);

	res_->localPos_ = {
			{-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
			{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}
	};
	res_->texcoord_ = {
		{0.0f, 0.0f}, {1.0f, 0.0f},
		{0.0f, 1.0f}, {1.0f, 1.0f},
		{1.0f, 0.0f}, {0.0f, 0.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}
	};
	res_->normal_ = {
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}
	};
	res_->index_ = {
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

	res_->psoConfig_.ps = "Game/Block.PS.hlsl";
	res_->psoConfig_.vs = "Game/Block.VS.hlsl";

	speed_.resize(instanceNum, 0.0f);
}

void DeathParticle::Initialize(Camera* camera) {
	
	const float minDistance = maxDistance_ / 3.0f;
	const float randRange = maxDistance_ - minDistance;

	res_->camera_ = camera;

	for (int i = 0; i < instanceNum; ++i) {
		Vector3 dir = { float(rand() % 2000 - 1000),
						float(rand() % 2000 - 1000),
						float(rand() % 2000 - 1000) };
		dir = dir.Normalize();
		float distance = float(rand() % 10000) / 10000.0f * randRange + minDistance;
		distance_.push_back(distance);
		Vector3 buff = dir * distance;
		res_->position_[i] = *playerPos_ + Vector3(buff.x, buff.y, buff.z) * std::numbers::pi_v<float>;
		res_->rotate_[i] = { rand() % 10000 / 1000.0f, rand() % 10000 / 1000.0f, rand() % 10000 / 1000.0f };
		res_->color_[i] = 0xff;
	}

	isInited_ = false;
}

void DeathParticle::Update(float deltaTime) {
	if (!isInited_) {
		for(int i = 0; i < instanceNum; ++i) {
			if (res_->color_[i] == 0) continue;

			Vector3 dir = res_->position_[i] - *playerPos_;
			dir = dir.Normalize();
			speed_[i] += acceleration_ * deltaTime;
			distance_[i] -= speed_[i] * deltaTime;
			res_->position_[i] = *playerPos_ + dir * distance_[i];
			res_->rotate_[i].y += 0.1f;

			if (distance_[i] < 0.0f) {
				res_->color_[i] = 0;
				++count;
			}
		}

		if (count == instanceNum) {
			isInited_ = true;
		}
	}

	if (isBoot_) {
		for (int i = 0; i < instanceNum; ++i) {
			if (res_->color_[i] == 0) continue;
			Vector3 dir = res_->position_[i] - *playerPos_;
			speed_[i] -= acceleration_ * deltaTime;
			res_->position_[i] = *playerPos_ + dir * speed_[i];

			if (speed_[i] <= 0.0f) {
				res_->color_[i] = 0;
				--count;
			}
		}

		if (count == 0) {
			isBoot_ = false;
			booted_ = true;
		}
	}
}

void DeathParticle::Draw(Render* render) {
	render->Draw(res_.get());
}

void DeathParticle::Boot() {
	if (isBoot_) return;

	isBoot_ = true;
	count = instanceNum;
	speed_.clear();
	for (int i = 0; i < instanceNum; ++i) {
		speed_[i] = float((rand() % 10000) / 2000) + 5.0f;
		res_->color_[i] = 0xffffffff;
	}
}

bool DeathParticle::isFin() {
	return booted_;
}
