#include "BackgroundParticleEffect.h"
#include <cmath>
#include <numbers>

BackgroundParticleEffect::BackgroundParticleEffect() {
	// 乱数エンジンの初期化
	std::random_device rd;
	randomEngine_ = std::mt19937(rd());
}

BackgroundParticleEffect::~BackgroundParticleEffect() {
}

void BackgroundParticleEffect::Initialize(Camera* camera, TextureManager* textureManager) {
	// 円形パーティクルの初期化
	{
		circleParticles_ = std::make_unique<ParticleResource>();
		circleParticles_->Initialize(4, 6, kMaxCircleParticles, false);
		circleParticles_->IsBillboard(true);
		circleParticles_->camera_ = camera;
		
		int textureHandle = textureManager->LoadTexture("Assets/Texture/circle.png");
		circleParticles_->textureHandle_ = textureHandle;
		
		// 通常ブレンドで神秘的な雰囲気に
		circleParticles_->psoConfig_.blendID = BlendStateID::Add;
		circleParticles_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
		
		circleParticles_->localPos_ = {
			{-0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f},
		};
		
		circleParticles_->texcoord_ = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
		};
		
		circleParticles_->normal_ = {
			{0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, -1.0f},
		};
		
		circleParticles_->index_ = {
			0, 1, 2,
			1, 3, 2,
		};
		
		circleLifeTime_.resize(kMaxCircleParticles);
		circleMaxLifeTime_.resize(kMaxCircleParticles);
		circleVelocity_.resize(kMaxCircleParticles);
		circleInitialPosition_.resize(kMaxCircleParticles);
		circleInitialScale_.resize(kMaxCircleParticles);
		circleRotationSpeed_.resize(kMaxCircleParticles);
		
		for (int i = 0; i < kMaxCircleParticles; ++i) {
			InitializeCircleParticle(i);
		}
	}
	
	// 四角形パーティクルの初期化
	{
		boxParticles_ = std::make_unique<ParticleResource>();
		boxParticles_->Initialize(4, 6, kMaxBoxParticles, false);
		boxParticles_->IsBillboard(false);  // 回転を見せるためビルボードオフ
		boxParticles_->camera_ = camera;
		
		int textureHandle = textureManager->LoadTexture("Assets/Texture/box.png");
		boxParticles_->textureHandle_ = textureHandle;
		
		// 加算合成でミステリアスな光の効果
		boxParticles_->psoConfig_.blendID = BlendStateID::Add;
		boxParticles_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
		
		boxParticles_->localPos_ = {
			{-0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f},
		};
		
		boxParticles_->texcoord_ = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
		};
		
		boxParticles_->normal_ = {
			{0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, -1.0f},
		};
		
		boxParticles_->index_ = {
			0, 1, 2,
			1, 3, 2,
		};
		
		boxLifeTime_.resize(kMaxBoxParticles);
		boxMaxLifeTime_.resize(kMaxBoxParticles);
		boxVelocity_.resize(kMaxBoxParticles);
		boxInitialPosition_.resize(kMaxBoxParticles);
		boxInitialScale_.resize(kMaxBoxParticles);
		boxRotationSpeed_.resize(kMaxBoxParticles);
		boxRotation_.resize(kMaxBoxParticles);
		
		for (int i = 0; i < kMaxBoxParticles; ++i) {
			InitializeBoxParticle(i);
		}
	}
}

void BackgroundParticleEffect::Update(float deltaTime) {
	globalTimer_ += deltaTime;
	
	// 円形パーティクルの更新
	for (int i = 0; i < kMaxCircleParticles; ++i) {
		circleLifeTime_[i] += deltaTime;
		
		if (circleLifeTime_[i] >= circleMaxLifeTime_[i]) {
			RespawnCircleParticle(i);
		}
		
		// 波動のような動き（サイン波で揺らぐ）
		float wave = std::sin(globalTimer_ * 0.5f + i * 0.3f) * 5.0f;
		circleParticles_->position_[i] += circleVelocity_[i] * deltaTime;
		circleParticles_->position_[i].x += wave * deltaTime;
		
		// アルファ値の計算（ゆっくりとしたフェード）
		float normalizedLife = circleLifeTime_[i] / circleMaxLifeTime_[i];
		float alpha = 1.0f;
		
		if (normalizedLife < 0.3f) {
			alpha = normalizedLife / 0.3f;
		} else if (normalizedLife > 0.7f) {
			alpha = (1.0f - normalizedLife) / 0.3f;
		}
		
		// ミステリアスな点滅効果
		float flicker = 0.8f + 0.2f * std::sin(circleLifeTime_[i] * 3.0f + i);
		alpha *= flicker;
		
		// スケールのパルス効果
		float pulse = 1.0f + 0.15f * std::sin(circleLifeTime_[i] * 1.5f);
		float scale = circleInitialScale_[i] * pulse;
		circleParticles_->scale_[i] = { scale, scale, scale };
		
		// モノトーンでミステリアスな色
		uint32_t alphaValue = static_cast<uint32_t>(alpha * 200.0f);  // 最大値を抑える
		
		// 青白い幽玄な色合い
		uint32_t r = 80 + (i * 7) % 30;
		uint32_t g = 100 + (i * 11) % 40;
		uint32_t b = 140 + (i * 13) % 60;
		
		circleParticles_->color_[i] = 
			(r << 24) | (g << 16) | (b << 8) | alphaValue;
	}
	
	// 四角形パーティクルの更新
	for (int i = 0; i < kMaxBoxParticles; ++i) {
		boxLifeTime_[i] += deltaTime;
		
		if (boxLifeTime_[i] >= boxMaxLifeTime_[i]) {
			RespawnBoxParticle(i);
		}
		
		// 螺旋状の動き
		float spiral = boxLifeTime_[i] * 0.3f;
		float spiralRadius = 20.0f * std::sin(boxLifeTime_[i] * 0.5f);
		Vector3 spiralOffset = {
			spiralRadius * std::cos(spiral),
			spiralRadius * std::sin(spiral),
			0.0f
		};
		
		boxParticles_->position_[i] += boxVelocity_[i] * deltaTime + spiralOffset * deltaTime;
		
		// 幾何学的な回転
		boxRotation_[i].x += boxRotationSpeed_[i] * deltaTime;
		boxRotation_[i].y += boxRotationSpeed_[i] * 0.7f * deltaTime;
		boxRotation_[i].z += boxRotationSpeed_[i] * 1.3f * deltaTime;
		boxParticles_->rotate_[i] = boxRotation_[i];
		
		// アルファ値の計算
		float normalizedLife = boxLifeTime_[i] / boxMaxLifeTime_[i];
		float alpha = 1.0f;
		
		if (normalizedLife < 0.25f) {
			alpha = normalizedLife / 0.25f;
		} else if (normalizedLife > 0.75f) {
			alpha = (1.0f - normalizedLife) / 0.25f;
		}
		
		// より強い点滅効果
		float flicker = 0.6f + 0.4f * std::sin(boxLifeTime_[i] * 5.0f + i * 1.5f);
		alpha *= flicker;
		
		// スケール
		float scale = boxInitialScale_[i];
		boxParticles_->scale_[i] = { scale, scale, scale };
		
		// 幾何学的な色（エッジの効いた色）
		uint32_t alphaValue = static_cast<uint32_t>(alpha * 180.0f);
		
		// シアン系の神秘的な色
		uint32_t r = 40 + (i * 5) % 20;
		uint32_t g = 120 + (i * 7) % 50;
		uint32_t b = 180 + (i * 11) % 40;
		
		boxParticles_->color_[i] = 
			(r << 24) | (g << 16) | (b << 8) | alphaValue;
	}
}

void BackgroundParticleEffect::Draw(Render* render) {
	// 先に円形を描画（背面）
	render->Draw(circleParticles_.get());
	// その後に四角形を描画（前面）
	render->Draw(boxParticles_.get());
}

void BackgroundParticleEffect::InitializeCircleParticle(int index) {
	std::uniform_real_distribution<float> xDist(-kScreenWidth * 0.6f, kScreenWidth * 0.6f);
	std::uniform_real_distribution<float> yDist(-kScreenHeight * 0.5f, kScreenHeight * 0.5f);
	std::uniform_real_distribution<float> zDist(70.0f, 140.0f);
	std::uniform_real_distribution<float> lifeDist(kMinLifeTime, kMaxLifeTime);
	std::uniform_real_distribution<float> scaleDist(kMinCircleScale, kMaxCircleScale);
	std::uniform_real_distribution<float> speedDist(-kMoveSpeed * 0.5f, kMoveSpeed * 0.5f);
	
	circleInitialPosition_[index] = {
		xDist(randomEngine_),
		yDist(randomEngine_),
		zDist(randomEngine_)
	};
	
	circleParticles_->position_[index] = circleInitialPosition_[index];
	
	circleMaxLifeTime_[index] = lifeDist(randomEngine_);
	std::uniform_real_distribution<float> startLifeDist(0.0f, circleMaxLifeTime_[index]);
	circleLifeTime_[index] = startLifeDist(randomEngine_);
	
	circleInitialScale_[index] = scaleDist(randomEngine_);
	
	// ゆっくりとした動き
	circleVelocity_[index] = {
		speedDist(randomEngine_),
		speedDist(randomEngine_) * 0.3f,
		0.0f
	};
	
	circleParticles_->rotate_[index] = { 0.0f, 0.0f, 0.0f };
}

void BackgroundParticleEffect::RespawnCircleParticle(int index) {
	std::uniform_real_distribution<float> xDist(-kScreenWidth * 0.6f, kScreenWidth * 0.6f);
	std::uniform_real_distribution<float> yDist(-kScreenHeight * 0.5f, kScreenHeight * 0.5f);
	std::uniform_real_distribution<float> zDist(70.0f, 140.0f);
	std::uniform_real_distribution<float> lifeDist(kMinLifeTime, kMaxLifeTime);
	std::uniform_real_distribution<float> scaleDist(kMinCircleScale, kMaxCircleScale);
	std::uniform_real_distribution<float> speedDist(-kMoveSpeed * 0.5f, kMoveSpeed * 0.5f);
	
	circleLifeTime_[index] = 0.0f;
	circleMaxLifeTime_[index] = lifeDist(randomEngine_);
	
	int edge = rand() % 4;
	switch (edge) {
		case 0: // 左
			circleInitialPosition_[index] = {
				-kScreenWidth * 0.6f,
				yDist(randomEngine_),
				zDist(randomEngine_)
			};
			break;
		case 1: // 右
			circleInitialPosition_[index] = {
				kScreenWidth * 0.6f,
				yDist(randomEngine_),
				zDist(randomEngine_)
			};
			break;
		case 2: // 上
			circleInitialPosition_[index] = {
				xDist(randomEngine_),
				kScreenHeight * 0.5f,
				zDist(randomEngine_)
			};
			break;
		case 3: // 下
			circleInitialPosition_[index] = {
				xDist(randomEngine_),
				-kScreenHeight * 0.5f,
				zDist(randomEngine_)
			};
			break;
	}
	
	circleParticles_->position_[index] = circleInitialPosition_[index];
	circleInitialScale_[index] = scaleDist(randomEngine_);
	
	Vector3 toCenter = {
		-circleInitialPosition_[index].x * 0.05f,
		-circleInitialPosition_[index].y * 0.03f,
		0.0f
	};
	
	circleVelocity_[index] = {
		speedDist(randomEngine_) + toCenter.x,
		speedDist(randomEngine_) * 0.3f + toCenter.y,
		0.0f
	};
}

void BackgroundParticleEffect::InitializeBoxParticle(int index) {
	std::uniform_real_distribution<float> xDist(-kScreenWidth * 0.5f, kScreenWidth * 0.5f);
	std::uniform_real_distribution<float> yDist(-kScreenHeight * 0.4f, kScreenHeight * 0.4f);
	std::uniform_real_distribution<float> zDist(60.0f, 120.0f);
	std::uniform_real_distribution<float> lifeDist(kMinLifeTime, kMaxLifeTime);
	std::uniform_real_distribution<float> scaleDist(kMinBoxScale, kMaxBoxScale);
	std::uniform_real_distribution<float> speedDist(-kMoveSpeed * 0.3f, kMoveSpeed * 0.3f);
	std::uniform_real_distribution<float> rotDist(-kRotationSpeed, kRotationSpeed);
	
	boxInitialPosition_[index] = {
		xDist(randomEngine_),
		yDist(randomEngine_),
		zDist(randomEngine_)
	};
	
	boxParticles_->position_[index] = boxInitialPosition_[index];
	
	boxMaxLifeTime_[index] = lifeDist(randomEngine_);
	std::uniform_real_distribution<float> startLifeDist(0.0f, boxMaxLifeTime_[index]);
	boxLifeTime_[index] = startLifeDist(randomEngine_);
	
	boxInitialScale_[index] = scaleDist(randomEngine_);
	
	boxVelocity_[index] = {
		speedDist(randomEngine_),
		speedDist(randomEngine_) * 0.5f,
		0.0f
	};
	
	boxRotationSpeed_[index] = rotDist(randomEngine_);
	boxRotation_[index] = { 
		static_cast<float>(rand() % 360) * std::numbers::pi_v<float> / 180.0f,
		static_cast<float>(rand() % 360) * std::numbers::pi_v<float> / 180.0f,
		static_cast<float>(rand() % 360) * std::numbers::pi_v<float> / 180.0f
	};
}

void BackgroundParticleEffect::RespawnBoxParticle(int index) {
	std::uniform_real_distribution<float> xDist(-kScreenWidth * 0.5f, kScreenWidth * 0.5f);
	std::uniform_real_distribution<float> yDist(-kScreenHeight * 0.4f, kScreenHeight * 0.4f);
	std::uniform_real_distribution<float> zDist(60.0f, 120.0f);
	std::uniform_real_distribution<float> lifeDist(kMinLifeTime, kMaxLifeTime);
	std::uniform_real_distribution<float> scaleDist(kMinBoxScale, kMaxBoxScale);
	std::uniform_real_distribution<float> speedDist(-kMoveSpeed * 0.3f, kMoveSpeed * 0.3f);
	std::uniform_real_distribution<float> rotDist(-kRotationSpeed, kRotationSpeed);
	
	boxLifeTime_[index] = 0.0f;
	boxMaxLifeTime_[index] = lifeDist(randomEngine_);
	
	int edge = rand() % 4;
	switch (edge) {
		case 0: // 左
			boxInitialPosition_[index] = {
				-kScreenWidth * 0.5f,
				yDist(randomEngine_),
				zDist(randomEngine_)
			};
			break;
		case 1: // 右
			boxInitialPosition_[index] = {
				kScreenWidth * 0.5f,
				yDist(randomEngine_),
				zDist(randomEngine_)
			};
			break;
		case 2: // 上
			boxInitialPosition_[index] = {
				xDist(randomEngine_),
				kScreenHeight * 0.4f,
				zDist(randomEngine_)
			};
			break;
		case 3: // 下
			boxInitialPosition_[index] = {
				xDist(randomEngine_),
				-kScreenHeight * 0.4f,
				zDist(randomEngine_)
			};
			break;
	}
	
	boxParticles_->position_[index] = boxInitialPosition_[index];
	boxInitialScale_[index] = scaleDist(randomEngine_);
	
	Vector3 toCenter = {
		-boxInitialPosition_[index].x * 0.03f,
		-boxInitialPosition_[index].y * 0.02f,
		0.0f
	};
	
	boxVelocity_[index] = {
		speedDist(randomEngine_) + toCenter.x,
		speedDist(randomEngine_) * 0.5f + toCenter.y,
		0.0f
	};
	
	boxRotationSpeed_[index] = rotDist(randomEngine_);
}
