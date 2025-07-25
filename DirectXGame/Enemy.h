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
#include <map>

struct EnemyCommand {
	//関数名
	std::string funcName;
	//関数の引数
	std::vector<std::shared_ptr<Value>> args;
	//timecallに設定したときのid(使わない時もある)
	int funcId = -1;

	EnemyCommand() = default;
	EnemyCommand(const EnemyCommand&) = default;
	EnemyCommand& operator=(const EnemyCommand&) = default;
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
	Enemy(Camera* camera, EnemyInfo queue, Transform transform, std::function<void(EnemyBulletDesc)> Fire);
	virtual ~Enemy() = default;

	static void RegistCommands();
	void Initialize() override;

	void Update() override;

	void OnCollision(Object* object) override;

	bool GetIsAlive() const { return isActive_; }

	void SetVeloity(const Vector3& v) { velocity_ = v; }

private:

	void AddFireDesc(EnemyCommand& command);
	void Death() { isActive_ = false; }
	void Wait(EnemyCommand& command);
	void Accel(EnemyCommand& command);
	void Move(EnemyCommand& command);

	//以下未実装
	void Delete(EnemyCommand& command);
	void Goto(EnemyCommand& command);

	std::function<void()> BulletDescSelector_ = nullptr;
	std::function<void(EnemyBulletDesc)> Fire_;

	//timecall
	std::unique_ptr<TimeCall> timecall_ = nullptr;

	//行動キュー関係
	EnemyInfo info_;
	int executeIndex_ = 0; //現在実行中のコマンドのインデックス
	int waitTime_ = 0;
	bool wating_ = false;

	//Command系
	static std::unordered_map<std::string, std::function<void(Enemy* e, EnemyCommand& command)>> commandMap_;
	static bool isCommandMapInitialized_;
};

class AccelAct : public Action {
public:

	AccelAct(Enemy* enemy, std::vector<std::shared_ptr<Value>> args);
	~AccelAct();

	void Execute() override;
	
	bool ShouldKeep() override;

private:

	Vector3 acceleration_ = {};
	Vector3 targetVelocity_ = {};
	Vector3 addedVelocity_ = {};

	bool isAccelerating_ = false;

	Enemy* enemy_;
};
