#pragma once
#include "../BaseObject.h"



class GoalTape : public BaseObject {
public:

	GoalTape();
	~GoalTape();

	void Initialize(int textureHandle, float positionX, float scaleY, PhysicsEngine* engine, Camera* camera);
	void Update(float deltaTime) override;
	void Draw(Render* render) override;

	bool GetClear() const { return isClear_; }

private:

	std::unique_ptr<DrawResource> drawResource_ = nullptr;

	bool isClear_ = false;

};
