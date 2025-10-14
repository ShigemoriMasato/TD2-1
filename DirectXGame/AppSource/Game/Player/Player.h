#pragma once
#include"../BaseObject.h"
#include <optional>
#include <Common/KeyConfig/KeyManager.h>

class Wire;
class Player : public BaseObject
{
public:

	Player();
	~Player();

	void Initialize(ModelData* modelData, Camera* camera)override;
	void Update(float deltaTime)override;
	void Draw(Render* render)override;

	void SetWire(Wire* wire) { wire_ = wire; }
	void OnCollision(BaseObject* other)override;

	void SetTargets(const std::list<BaseObject*>* targets) { targets_ = targets; }
	void SetKeyConfig(std::unordered_map<Key, bool>* keyConfig) { key_ = keyConfig; }
private:
	enum class Behavior
	{
		Extend,			//伸ばす
		Shrink,			//縮める
		Idel,			//待機(着地)
		Hanging,		//吊り下げ
		Dash,			//ダッシュ
	};

private://状態変数
	//プレイヤー状態
	Behavior* behavior_ = nullptr;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

private://パラメータ
	float moveSpeed_ = 10.0f;

private:
	Wire* wire_ = nullptr;
	const std::list<BaseObject*>* targets_ = nullptr;
	std::unordered_map<Key, bool>* key_ = nullptr;
private://メンバ関数

};

