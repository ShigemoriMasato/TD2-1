#pragma once
#include <vector>
#include <memory>

class Action;

enum class ActionType {
	unique,
	share
};

class Actor {
public:
	virtual ~Actor() = default;

	void EnqueueAction(std::shared_ptr<Action> act, ActionType actiontype = ActionType::unique);
	void ExecuteQueue();

private:

	std::vector<std::shared_ptr<Action>> actions_;

};
