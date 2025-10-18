#pragma once
#include"../BaseObject.h"
#include <optional>
#include <Common/KeyConfig/KeyManager.h>
#include <Game/FPS/TimeSlower.h>

; /* このセミコロンを消したらエラーが出る。 */
class Wire;
class Player : public BaseObject
{
public:

	Player(TimeSlower* slower);
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
		Idle,			//待機(着地)
		Forcus,			//狙いを定める
		Extend,			//伸ばす
		Shrink,		//縮める
		Dash,			//ダッシュ
	};

private://状態変数

	//プレイヤー状態
	Behavior behavior_ = Behavior::Dash;
	Behavior behaviorPrev_ = Behavior::Dash;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

private://パラメータ
	Vector3 velocity_ = {};

	//wireを投げた場所
	Vector3 targetPos_ = {};
	Vector3 targetDir_ = {};

	//移動速度
	const float moveSpeed_ = 10.0f;
	//ダッシュ中のキーによる移動速度
	const float dashMoveSpeed_ = 5.0f;

	//重力
	const float gravity_ = -9.8f;
	//ワイヤーを伸ばしてる時の重力適用率
	const float extendGravityRate_ = 0.1f;
	//ワイヤーで引っ張った時の最初の速度
	const float dashPower_ = 20.0f;
	//dashの速度軽減率
	const float dashRegistRate_ = 0.99f;

	//wireを再び伸ばせるまでのクールタイム
	float wireCoolTime_ = 0.0f;
	const float maxWireCoolTime_ = 0.5f;

	//? ImGui用 debug
	std::unordered_map<Behavior, std::string> behMap = {
		{Behavior::Idle, "Idle"},
		{Behavior::Forcus, "Forcus"},
		{Behavior::Extend, "Extend"},
		{Behavior::Shrink, "Shrink"},
		{Behavior::Dash, "Dash"}
	};

private:

	Wire* wire_ = nullptr;
	const std::list<BaseObject*>* targets_ = nullptr;
	std::unordered_map<Key, bool>* key_ = nullptr;

	TimeSlower* timeSlower_ = nullptr;

private://メンバ関数
	//ビヘイビアリクエスト
	void RequestBehavior();

	static void (Player::*behaviorUpdate[])(float);
	static void (Player::*behaviorOn[])();

	//プレイヤー待機
	void OnIdel();
	void UpdateIdel(float deltaTime);

	//ワイヤーの狙いを定める
	void OnForcus();
	void UpdateForcus(float deltaTime);

	//ワイヤーを伸ばす
	void OnExtend();
	void UpdateExtend(float deltaTime);

	//ワイヤーを縮める
	void OnShrink();
	void UpdateShrink(float deltaTime);

    //プレイヤーダッシュ
    void OnDash();
    void UpdateDash(float deltaTime);
};

