#pragma once
#include "Object/Action.h"
#include "Object/Actor.h"

class SampleGraph : public Actor {
public:

	SampleGraph(Camera* camera, int* beat);
	~SampleGraph() override = default;

	void Initialize() override;
	void Update() override;
	void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

private:
	Camera* camera_ = nullptr;
	int* beat_ = nullptr;
	int preBeat_ = 0;
};

