#pragma once
#include <Render/Render.h>

class Player;
class KeyManager;

class TargetScope {
public:

	TargetScope();
	~TargetScope();

	void Initialize(int textureHandle, Player* player, Camera* camera);
	void Update(float deltaTime, KeyManager* keyManager);
	void Draw(class Render* render);

private:

	std::unique_ptr<DrawResource> scope_ = nullptr;
	std::unique_ptr<DrawResource> sphere_ = nullptr;

	Vector3* playerPosition_ = nullptr;
	Player* player_ = nullptr;
	Vector3 offset_ = { 2.0f, 0.0f, 0.0f };
};
