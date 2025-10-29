#include "DeathParticle.h"

DeathParticle::DeathParticle(Vector3* playerPos) {
	playerPos_ = playerPos;
	res_ = std::make_unique<ParticleResource>();
	res_->Initialize(8, 36, 200, true);

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

}

void DeathParticle::Initialize() {
	
	const float minDistance = maxDistance_ / 3.0f;
	const float randRange = maxDistance_ - minDistance;

	for (int i = 0; i < instanceNum; ++i) {
		Vector2 dir = {float(rand() % 2000 - 1000) / 1000.0f,
						float(rand() % 2000 - 1000) / 1000.0f };
		float distance = float(rand() % 10000) / minDistance + minDistance;
		distance_.push_back(distance);
		Vector2 buff = dir * distance;
		res_->position_[i] = *playerPos_ + Vector3(buff.x, buff.y, 0.0f) * std::numbers::pi_v<float>;
	}

	isInited_ = false;
}

void DeathParticle::Update(float deltaTime) {
	if (isInited_) {
		for(int i = 0; i < instanceNum; ++i) {
			Vector3 dir = res_->position_[i] - *playerPos_;
			dir = dir.Normalize();
			speed_[i] += acceleration_ * deltaTime;
			distance_[i] -= speed_[i] * deltaTime;
			res_->position_[i] = *playerPos_ + dir * distance_[i];

			if (distance_[i] < 0.0f) {
				res_->color_[i] = 0;
				++count;
			}
		}

		if (count == instanceNum) {
			isInited_ = false;
		}
	}

	if (isBoot_) {
		for (int i = 0; i < instanceNum; ++i) {
			if (res_->color_[i] == 0) continue;
			Vector3 dir = res_->position_[i] - *playerPos_;
			dir = dir.Normalize();
			speed_[i] -= acceleration_ * deltaTime;
			distance_[i] += speed_[i] * deltaTime;
			res_->position_[i] = *playerPos_ + dir * distance_[i];

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

void DeathParticle::Render() {
}

void DeathParticle::Boot() {
	isBoot_ = true;
	count = instanceNum;
	speed_.clear();
	for (int i = 0; i < instanceNum; ++i) {
		speed_.push_back(rand() % 10000 / 2000 + 3.0f);
	}
}

bool DeathParticle::isFin() {
	return booted_;
}
