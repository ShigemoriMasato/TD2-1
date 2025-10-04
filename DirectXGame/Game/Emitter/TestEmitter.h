#pragma once
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include <random>
#include <cstdint>

enum class ParticleJob : uint32_t {
	UnBreakable = 1 << 0,		//壊れない
	ToTransparent = 1 << 1,		//だんだん透明になっていく
	Scatter = 1 << 2,			//四方に散らばる

};

uint32_t operator|(ParticleJob a, ParticleJob b);

uint32_t operator|(uint32_t a, ParticleJob b);

uint32_t operator&(uint32_t a, ParticleJob b);

class TestEmitter {
public:

	TestEmitter(int maxNum);
	~TestEmitter();

	void Initialize(Camera* camera);
	void Update();
	void Draw(Render* render);

	void SetLifeTime(int time);
	void SetJobs(uint16_t jobs);
	void SetPosition(Vector3 pos);

	void SetGenerateNum(int num) { generateNum_ = num; }

private:

	const int maxNum_;

	//Emitterに登録されているパーティクルの寿命
	int lifeTime_ = 120;

	std::mt19937 randEngine_;

	std::unique_ptr<ParticleResource> particle_ = nullptr;

	int particleNum_ = 0;
	std::vector<Vector3> velocity_{};
	//パーティクル一個ごとの生存時間
	std::vector<int> life_{};
	//パーティクル一個ごとの寿命
	std::vector<int> lifeSpan_{};

	//Emitterに登録されているパーティクルの動作
	uint16_t emitterJobs_ = 0;
	//パーティクル一個ずつに登録されているパーティクルの動作
	std::vector<uint16_t> particleJobs_{};

	//発生場所
	Vector3 position_{ 1.0f, 1.0f, 1.0f };

	int frame_;
	int coolTime_ = 2;

	//一度に生成するパーティクルの数
	int generateNum_ = 1;
};
