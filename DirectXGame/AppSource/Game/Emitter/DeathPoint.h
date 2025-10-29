#pragma once
#include <Render/Render.h>

class DeathPoint {
public:

	void Initialize(std::vector<Vector3> deathPoints, int textureHandle, Camera* camera);

	void Draw(Render* render);

private:

	float size_ = 1.0f;
	std::unique_ptr<ParticleResource> res_;

};

