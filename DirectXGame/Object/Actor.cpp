#include "Actor.h"
#include "Action.h"

void Actor::EnqueueAction(std::shared_ptr<Action> act, ActionType actiontype) {
	//実行可能でなければ何もしない
	if (!act->CanExecute()) {
		return;
	}

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
	for (int i = 0; i < actions_.size(); ++i) {
		//登録されたアクションを実行
		actions_[i]->Execute();

		//アクションが終了していたら削除
		if (!actions_[i]->ShouldKeep()) {
			actions_.erase(actions_.begin() + i--);
		}
	}
}
