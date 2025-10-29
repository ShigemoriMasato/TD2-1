#pragma once
#include <Render/Render.h>

class BackGround {
public:

	BackGround();
	~BackGround() = default;

	void Initialize(Camera* camera);
	void Update(float deltaTime);
	void Draw(Render* render);

private:

	std::vector<std::vector<std::pair<int, float>>> blockMap_{};

	std::unique_ptr<ParticleResource> blocks_ = nullptr;
	std::unique_ptr<PostEffectResource> postEffect_ = nullptr;

	const float changeInterval = 3.5f;
	float timer = changeInterval;

	const int width = 50;
	const int height = 16;

};
