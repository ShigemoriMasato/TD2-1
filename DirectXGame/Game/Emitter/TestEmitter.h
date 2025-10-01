#pragma once
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include <random>

class TestEmitter {
public:

	TestEmitter(int maxNum);
	~TestEmitter();
	void Initialize(Camera* camera);
	void Update();
	void Draw(Render* render);

private:

	const int maxNum_;

	std::mt19937 randEngine_;

	std::unique_ptr<ParticleResource> particle_ = nullptr;

	int particleNum_ = 0;
	std::vector<Vector3> velocity_{};
	std::vector<int> life_{};

	int frame_;
	const int coolTime_ = 2;
};
