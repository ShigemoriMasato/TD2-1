#include "Enemy.h"
#include <numbers>
#include <cassert>

std::unordered_map<std::string, std::function<void(Enemy* e, EnemyCommand& command)>> Enemy::commandMap_;
bool Enemy::isCommandMapInitialized_ = false;

Enemy::Enemy(Camera* camera, EnemyInfo info, Transform transform, std::function<void(EnemyBulletDesc)> Fire) :
Actor(camera, ShapeType::Model),
Fire_(Fire),
timecall_(std::make_unique<TimeCall>()) {
	handle_ = info.handle;
	tag = "Enemy";
	info_ = info;

	*transform_ = transform;

	if(info_.queue.size() <= 0) {
		assert(false && "EnemyQueue is Empty");
	}
}

void Enemy::RegistCommands() {
	if (isCommandMapInitialized_) return;

	isCommandMapInitialized_ = true;

	commandMap_["Fire"] = [](Enemy* e, EnemyCommand& command) {
		e->AddFireDesc(command);
		};
	commandMap_["Death"] = [](Enemy* e, EnemyCommand& command) {
		e->Death();
		};
	commandMap_["Wait"] = [](Enemy* e, EnemyCommand& command) {
		e->Wait(command);
		};
	commandMap_["Accel"] = [](Enemy* e, EnemyCommand& command) {
		e->Accel(command);
		};
	commandMap_["Delete"] = [](Enemy* e, EnemyCommand& command) {
		e->Delete(command);
		};
	commandMap_["Goto"] = [](Enemy* e, EnemyCommand& command) {
		e->Goto(command);
		};
	commandMap_["Move"] = [](Enemy* e, EnemyCommand& command) {
		e->Move(command);
		};

}

void Enemy::Initialize() {
}

void Enemy::Update() {
	while (!wating_) {

		if (executeIndex_ >= info_.queue.size()) {
			break; //キューが空なら何もしない
		}

		EnemyCommand& cmd = info_.queue[executeIndex_];
		commandMap_[cmd.funcName](this, cmd);
		++executeIndex_;
	}

	if (wating_) {
		Wait(info_.queue[executeIndex_]);
	}

	ExecuteQueue();
	timecall_->Update();
	transform_->position += velocity_;
}

void Enemy::OnCollision(Object* other) {
}

void Enemy::AddFireDesc(EnemyCommand& command) {
	int cooltime = -1;

	EnemyBulletDesc desc{};
	desc.localPosition = &transform_->position;
	for (auto& arg : command.args) {
		if (arg->name == "position") {
			desc.position += dynamic_cast<Vec3Value*>(arg.get())->value;
		} else if (arg->name == "direction") {
			desc.direction = dynamic_cast<Vec3Value*>(arg.get())->value;
		} else if (arg->name == "speed") {
			desc.speed = dynamic_cast<FloatValue*>(arg.get())->value;
		} else if (arg->name == "color") {
			Vector4 color = dynamic_cast<Vec4Value*>(arg.get())->value;

			desc.color = (
				(static_cast<uint32_t>(color.x * 255) << 24) |
				(static_cast<uint32_t>(color.y * 255) << 16) |
				(static_cast<uint32_t>(color.z * 255) << 8) |
				(static_cast<uint32_t>(color.w * 255))
				);

		} else if (arg->name == "is_tracking") {
			desc.isTracking = dynamic_cast<IntValue*>(arg.get())->value != 0;
		} else if (arg->name == "cooltime") {
			cooltime = dynamic_cast<IntValue*>(arg.get())->value;
		}
	}

	//cooltimeが-1でない場合は、時間経過で発射する
	if (cooltime != -1) {

		BulletDescSelector_ = [this, desc]() {
			Fire_(desc);
			};

		info_.queue[0].funcId = timecall_->Register(BulletDescSelector_, cooltime, true);

	} else {
		Fire_(desc);
	}
}

void Enemy::Wait(EnemyCommand& command) {
	//waitの初期化処理
	if (!wating_) {

		wating_ = true;

		if (command.args.size() <= 0) {
			waitTime_ = 60;
		} else {
			waitTime_ = dynamic_cast<IntValue*>(command.args[0].get())->value;
		}
	} 
	//waitの更新処理
	else {

		waitTime_--;

		if (waitTime_ <= 0) {
			wating_ = false;
		}

	}
}

void Enemy::Accel(EnemyCommand& command) {
	EnqueueAction(std::make_shared<AccelAct>(this, command.args));
}

void Enemy::Move(EnemyCommand& command) {

	for (auto& arg : command.args) {
		if (arg->name == "velocity") {
			velocity_ = dynamic_cast<Vec3Value*>(arg.get())->value;
		}
	}

}

void Enemy::Delete(EnemyCommand& command) {
	if (command.args.size() <= 0) {
		return;
	}

	for(auto& arg : command.args) {
		if (arg->name == "index") {
			int line = dynamic_cast<IntValue*>(arg.get())->value;
			if (line < 0 || line >= info_.queue.size()) {
				return;
			}

			int index = -1;
			for (int i = 0; i < info_.queueLines.size(); ++i) {
				if (line == info_.queueLines[i]) {
					index = i;
				}
			}

			//見つからなかった
			if (index == -1) {
				return;
			}

			if (info_.queue[index].funcName == "Fire") {
				//indexをもとにFireを削除する
			}
		}
	}
}

void Enemy::Goto(EnemyCommand& command) {
	for (auto& arg : command.args) {
		if (arg->name == "index") {
			executeIndex_ = dynamic_cast<IntValue*>(arg.get())->value;
			break;
		}
	}
}


AccelAct::AccelAct(Enemy* enemy, std::vector<std::shared_ptr<Value>> args) : Action("Accel", nullptr) {

	for (auto& arg : args) {
		if (arg->name == "acceleration") {
			Vector3 acceleration = dynamic_cast<Vec3Value*>(arg.get())->value;
			acceleration_ = acceleration;
		}

		if (arg->name == "velocity") {
			Vector3 velocity = dynamic_cast<Vec3Value*>(arg.get())->value;
			targetVelocity_ = velocity;
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (acceleration_[i] == 0.0f) {
			acceleration_[i] = 0.001f; //加速が0の場合は、微小な値を設定
		}
	}

	enemy_ = enemy;
	isAccelerating_ = true;
}

AccelAct::~AccelAct() {
}

void AccelAct::Execute() {
	Vector3 acceleration = acceleration_;
	addedVelocity_ += acceleration;

	int limited = 0;			//加速制限をかけられた回数

	for (int i = 0; i < 3; ++i) {
		if ((targetVelocity_[i] - addedVelocity_[i]) * acceleration[i] < 0) {
			acceleration[i] += targetVelocity_[i] - addedVelocity_[i];
			addedVelocity_[i] = targetVelocity_[i];
			++limited;
		}
	}

	if (limited == 3) {
		isAccelerating_ = false;
	}

	enemy_->AddVelocity(acceleration);
}

bool AccelAct::ShouldKeep() {
	return isAccelerating_;
}
