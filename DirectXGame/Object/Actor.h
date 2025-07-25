#pragma once
#include <vector>
#include <memory>
#include "Object.h"

class Action;

enum class ActionType {
	unique,
	share
};

class Actor : public Object {
public:
	Actor(Camera* camera, ShapeType type) : Object(camera, type) {}

	void EnqueueAction(std::shared_ptr<Action> act, ActionType actiontype = ActionType::share);
	void ExecuteQueue();

	void ExecuteDrawQueue() const;

	void ClearQueue();

private:

	std::vector<std::shared_ptr<Action>> actions_;

};
