#pragma once
#include "../BaseObject.h"



class GoalTape : public BaseObject {
public:

	GoalTape();
	~GoalTape();

	void Initialize(TextureManager* textureManager, float positionX, PhysicsEngine* engine, Camera* camera);
	void Update(float deltaTime) override;
	void Draw(Render* render) override;

private:

	std::unique_ptr<DrawResource> drawResource_ = nullptr;

};
