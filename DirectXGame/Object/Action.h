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
	/// 実行する関数
	/// </summary>
	virtual void Execute() = 0;

	/// <summary>
	/// Drawを実行したかったらここに記述する
	/// </summary>
	virtual void ExecuteDraw() {};

	/// <summary>
	/// キューに残り続けるか
	/// </summary>
	/// <returns>true:残る false:消える</returns>
	virtual bool ShouldKeep() { return false; };

	const std::string name_;
	std::shared_ptr<Actor> actor_;
};
