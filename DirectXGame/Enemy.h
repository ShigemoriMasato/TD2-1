#pragma once
#include "Object/Object.h"
#include "EnemyBullet.h"
#include "TimeCall.h"
#include "Object/Actor.h"
#include "Object/Action.h"
#include <memory>
#include <functional>
#include <Data/Value.h>
#include <unordered_map>

struct EnemyCommand {
	//関数名
	std::string funcName;
	//関数の引数
	std::vector<std::shared_ptr<Value>> args;
};

struct EnemyInfo {
	//敵の名前
	std::string name;
	int handle = -1; //敵のモデルハンドル

	//敵の行動キュー(初期化時に解読する)
	std::vector<EnemyCommand> queue;

	//敵の行動キューがどの行に記述されていたか(行動の削除など、他の行動に干渉する際に使用)
	std::vector<int> queueLines;
};

class Enemy : public Actor {
public:
	Enemy(Camera* camera, EnemyInfo queue, std::function<void(EnemyBulletDesc)> Fire);
	virtual ~Enemy() = default;

	static void RegistCommands();
	void Initialize() override;

	void Update() override;

	void OnCollision(Object* object) override;

	bool GetIsAlive() const { return isActive_; }

	void SetVeloity(const Vector3& v) { velocity_ = v; }

	void AddVelocity(const Vector3& v) { velocity_ += v; }

private:

	void AddFireDesc(std::vector<std::shared_ptr<Value>> args);
	void Death() { isActive_ = false; }
	void Wait(std::vector<std::shared_ptr<Value>> args);
	void Accel(std::vector<std::shared_ptr<Value>> args);

	std::function<void(EnemyBulletDesc)> Fire_;

	//敵について
	Vector3 velocity_ = {};

	//timecall
	std::unique_ptr<TimeCall> timecall_ = nullptr;

	//行動キュー関係
	EnemyInfo info_;
	int executeIndex_ = 0; //現在実行中のコマンドのインデックス
	int waitTime_ = 0;
	bool wating_ = false;

	//Command系
	static std::unordered_map<std::string, std::function<void(Enemy* e, std::vector<std::shared_ptr<Value>> args)>> commandMap_;
	static bool isCommandMapInitialized_;
};

class AccelAct : public Action {
public:

	AccelAct(Enemy* enemy, std::vector<std::shared_ptr<Value>> args);

	void Execute() override;
	
	bool ShouldKeep() override;

private:

	Vector3 acceleration_ = {};
	Vector3 targetVelocity_ = {};
	Vector3 addedVelocity_ = {};

	bool isAccelerating_ = false;

	Enemy* enemy_ = nullptr;

};
