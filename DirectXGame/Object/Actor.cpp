#include "Actor.h"
#include "Action.h"
#include "../externals/imgui/imgui.h"

void Actor::EnqueueAction(std::shared_ptr<Action> act, ActionType actiontype) {

	//アクションに一つしか入ってほしくないもので、すでに同じアクションが入っていたなら何もしない
	if (actiontype == ActionType::unique) {
		for (auto& a : actions_) {
			if (act->name_ == a->name_) {
				return;
			}
		}
	}

	//アクションを登録
	actions_.push_back(act);
	
}

void Actor::ExecuteQueue() {

	ImGui::Begin("MotionQueue");
	for (auto& a : actions_) {
		ImGui::Text("%s", a->name_.c_str());
	}
	ImGui::End();

	for (int i = 0; i < actions_.size(); ++i) {
		//登録されたアクションを実行
		actions_[i]->Execute();

		//アクションが終了していたら削除
		if (!actions_[i]->ShouldKeep()) {
			actions_.erase(actions_.begin() + i--);
		}
	}
}

void Actor::ExecuteDrawQueue() const {
	for (const auto& action : actions_) {
		action->ExecuteDraw();
	}
}

void Actor::ClearQueue() {
	actions_.clear();
}
