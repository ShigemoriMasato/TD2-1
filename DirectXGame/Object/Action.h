#pragma once
#include <string>
#include "Actor.h"
#include "../Engine/Data/Transform.h"

class Action {
public:
	//名前と動かす対象を指定する
	Action(std::string name, Actor* actor)	: name_(name), actor_(actor) {}
	virtual ~Action() = default;

	/// <summary>
	/// キューに入れられるかどうか(初期値true)
	/// </summary>
	/// <returns>true:入れる</returns>
	virtual bool CanExecute() { return true; };

	/// <summary>
	/// 実行する関数
	/// </summary>
	virtual void Execute() = 0;

	/// <summary>
	/// キューに残り続けるか
	/// </summary>
	/// <returns>true:残る false:消える</returns>
	virtual bool ShouldKeep() { return false; };

	const std::string name_;
	std::shared_ptr<Actor> actor_;
};
