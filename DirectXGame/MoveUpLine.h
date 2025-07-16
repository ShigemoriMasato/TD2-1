#pragma once
#include "Object/Action.h"
#include "Object/Object.h"
#include <vector>

class MoveUpLineObject : public Object {
public:
	MoveUpLineObject(Camera* camera, ShapeType type) : Object(camera, type) {}
	~MoveUpLineObject() override = default;

	void Initialize() {}
	void Update() {}

private:

};

class MoveUpLine : public Action {
public:

	MoveUpLine(Vector3 center, Vector3 end, int lifeTime, Camera* camera);
	~MoveUpLine() override = default;

	void Execute() override;

	void ExecuteDraw() override;

	bool ShouldKeep() override { return lifeTime_ < maxLifeTime_; };

private:

	std::unique_ptr<MoveUpLineObject> line_;

	Vector3 start_;
	Vector3 end_;
	const int lag_ = 0;

	int maxLifeTime_ = 0;
	int lifeTime_ = 0;
};
