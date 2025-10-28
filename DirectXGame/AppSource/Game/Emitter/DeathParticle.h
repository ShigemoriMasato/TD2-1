#pragma once
#include <Render/Render.h>

class DeathParticle {
public:

	DeathParticle(Vector3* playerPos);
	~DeathParticle() = default;

	void Initialize();
	void Update(float deltaTime);
	void Render();

	void Boot();

	bool isFin();

private:

	const int instanceNum = 200;

	Vector3* playerPos_;
	float timer_;
	bool isInited_;
	bool isBoot_;
	std::vector<float> distance_;
	std::vector<float> speed_;
	const float maxDistance_ = 40.0f;
	const float acceleration_ = 50.0f;
	std::unique_ptr<ParticleResource> res_;

};

