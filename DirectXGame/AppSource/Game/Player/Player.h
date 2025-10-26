#pragma once
#include"../BaseObject.h"
#include <optional>
#include <Common/KeyConfig/KeyManager.h>
#include <Game/FPS/TimeSlower.h>

class Wire;
class TileMap;
class Player : public BaseObject
{
public:

	enum class Behavior {
		Idle,			//待機(着地)
		Forcus,			//狙いを定める
		Extend,			//伸ばす
		Shrink,			//縮める
		Dash,			//ダッシュ
		Clear			//ゲームクリアしたときの挙動
	};

public:

	Player(TimeSlower* slower, PhysicsEngine* phEngine);
	~Player();

	void Initialize(ModelData* modelData, Camera* camera)override;
	void Update(float deltaTime)override;
	void Draw(Render* render)override;

	void SetWire(Wire* wire) { wire_ = wire; }
	void OnCollision(BaseObject* other)override;

	void AddTargets(BaseObject* target) { targets_.push_back(target); }
	void SetKeyConfig(std::unordered_map<Key, bool>* keyConfig) { key_ = keyConfig; }
	void SetTileMap(const TileMap* tileMap) { tileMap_ = tileMap; }
	void SetIsClear() { behaviorRequest_ = Behavior::Clear; }

	// 速度操作用のアクセッサ
	void AddVelocity(const Vector2& velocity) { 
		if (actor_) {
			actor_->velocity_ += velocity;
		}
	}
	Vector2 GetVelocity() const { 
		return actor_ ? actor_->velocity_ : Vector2{0.0f, 0.0f};
	}
	Behavior GetBehavior() const { return behavior_; }
	Vector3 GetDirection() const { return direction; }
	Vector3 GetTargetPos() const { return targetPos_; }

private://状態変数

	//プレイヤー状態
	Behavior behavior_ = Behavior::Dash;
	Behavior behaviorPrev_ = Behavior::Dash;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

private://パラメータ
	//wireを投げた場所
	Vector3 targetPos_ = {};
	Vector3 targetDir_ = {};

	//移動速度
	const float moveSpeed_ = 20.0f;
	//ダッシュ中のキーによる移動速度
	const float dashMoveSpeed_ = 10.0f;

	//重力
	const float gravity_ = -9.8f;
	//ワイヤーを伸ばしてる時の重力適用率
	const float extendGravityRate_ = 0.1f;
	//ワイヤーで引っ張った時の速度
	float dashPower_ = 0.0f;
	//ワイヤーで引っ張った時の加速度
	float dashAcceleration_ = 50.0f;
	//ワイヤーで引っ張った時の最大速度
	float maxDashSpeed_ = 30.0f;

	//dashの速度軽減率
	const float dashRegistRate_ = 0.97f;

	//wireを再び伸ばせるまでのクールタイム
	float wireCoolTime_ = 0.0f;
	const float maxWireCoolTime_ = 0.5f;

	const float rotateMaxSpeed_ = -20.0f;
	float rotateSpeed_ = 0.0f;

	//wireを伸ばしている方向
	Vector3 direction = { 0.0f, 0.0f, 0.0f };

	//? ImGui用 debug
	std::unordered_map<Behavior, std::string> behMap = {
		{Behavior::Idle, "Idle"},
		{Behavior::Forcus, "Forcus"},
		{Behavior::Extend, "Extend"},
		{Behavior::Shrink, "Shrink"},
		{Behavior::Dash, "Dash"},
		{Behavior::Clear, "Clear"},
	};

private:

	Wire* wire_ = nullptr;
	std::list<BaseObject*> targets_;
	std::unordered_map<Key, bool>* key_ = nullptr;

	TimeSlower* timeSlower_ = nullptr;
	const TileMap* tileMap_ = nullptr;
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

	//ゲームクリア時の挙動
	void OnClear();
	void UpdateClear(float deltaTime);

private://ワイヤー関連
	Vector3 GetInputDirection();
	BaseObject* SelectTargetByDirection(const Vector3& direction);
};

