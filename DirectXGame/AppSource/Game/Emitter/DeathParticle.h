#pragma once
#include <Render/Render.h>

class DeathParticle {
public:

	DeathParticle(Vector3* playerPos);
	~DeathParticle() = default;

	void Initialize(Camera* camera);
	void Update(float deltaTime);
	void Draw(Render* render);

	void Boot();

	bool isFin();

private:

	const int instanceNum = 50;

	int count = 0;

	Vector3* playerPos_;
	float timer_;
	bool isInited_ = false;
	bool isBoot_ = false;
	bool booted_ = false;
	std::vector<float> distance_;
	std::vector<float> speed_;
	const float maxDistance_ = 40.0f;
	const float acceleration_ = 50.0f;
	std::unique_ptr<ParticleResource> res_;

};

