#pragma once
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include <Camera/Camera.h>
#include <Resource/Texture/TextureManager.h>
#include <memory>
#include <vector>
#include <random>

/// <summary>
/// セレクトシーン背景用のパーティクルエフェクト
/// プリミティブでミステリアスな世界観に合わせた幾何学的なパーティクル
/// </summary>
class BackgroundParticleEffect {
public:
	BackgroundParticleEffect();
	~BackgroundParticleEffect();

	void Initialize(Camera* camera, TextureManager* textureManager);
	void Update(float deltaTime);
	void Draw(Render* render);

private:
	// パーティクル関連
	std::unique_ptr<ParticleResource> circleParticles_ = nullptr;
	std::unique_ptr<ParticleResource> boxParticles_ = nullptr;
	
	// パーティクルの最大数
	static constexpr int kMaxCircleParticles = 30;
	static constexpr int kMaxBoxParticles = 20;
	
	// パーティクルの寿命管理（円形）
	std::vector<float> circleLifeTime_;
	std::vector<float> circleMaxLifeTime_;
	std::vector<Vector3> circleVelocity_;
	std::vector<Vector3> circleInitialPosition_;
	std::vector<float> circleInitialScale_;
	std::vector<float> circleRotationSpeed_;
	
	// パーティクルの寿命管理（四角形）
	std::vector<float> boxLifeTime_;
	std::vector<float> boxMaxLifeTime_;
	std::vector<Vector3> boxVelocity_;
	std::vector<Vector3> boxInitialPosition_;
	std::vector<float> boxInitialScale_;
	std::vector<float> boxRotationSpeed_;
	std::vector<Vector3> boxRotation_;
	
	// 乱数生成
	std::mt19937 randomEngine_;
	
	// 画面範囲
	static constexpr float kScreenWidth = 1280.0f;
	static constexpr float kScreenHeight = 720.0f;
	
	// パーティクルのパラメータ
	static constexpr float kMinLifeTime = 4.0f;
	static constexpr float kMaxLifeTime = 12.0f;
	static constexpr float kMinCircleScale = 15.0f;
	static constexpr float kMaxCircleScale = 50.0f;
	static constexpr float kMinBoxScale = 8.0f;
	static constexpr float kMaxBoxScale = 30.0f;
	static constexpr float kMoveSpeed = 10.0f;
	static constexpr float kRotationSpeed = 0.5f;
	
	// グローバルタイマー（波動効果用）
	float globalTimer_ = 0.0f;
	
	// パーティクルの生成
	void InitializeCircleParticle(int index);
	void RespawnCircleParticle(int index);
	void InitializeBoxParticle(int index);
	void RespawnBoxParticle(int index);
};
