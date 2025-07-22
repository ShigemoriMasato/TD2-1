#include "Enemy.h"
#include <numbers>

std::unordered_map<std::string, std::function<void(Enemy* e, std::vector<std::shared_ptr<Value>> args)>> Enemy::commandMap_;
bool Enemy::isCommandMapInitialized_ = false;

Enemy::Enemy(Camera* camera, EnemyInfo info, std::function<void(EnemyBulletDesc)> Fire) :
Actor(camera, ShapeType::Model),
Fire_(Fire) {
	handle_ = info.handle;
	tag = "Enemy";
	info_ = info;
}

void Enemy::RegistCommands() {
	if (isCommandMapInitialized_) return;

	isCommandMapInitialized_ = true;

	commandMap_["Fire"] = [](Enemy* e, std::vector<std::shared_ptr<Value>> args) {
		e->AddFireDesc(args);
	};
	commandMap_["Death"] = [](Enemy* e, std::vector<std::shared_ptr<Value>> args) {
		e->Death();
	};
	commandMap_["Wait"] = [](Enemy* e, std::vector<std::shared_ptr<Value>> args) {
		e->Wait(args);
	};
	commandMap_["Accel"] = [](Enemy* e, std::vector<std::shared_ptr<Value>> args) {
		e->Accel(args);
	};
}

void Enemy::Initialize() {
}

void Enemy::Update() {
	while(!wating_) {
		commandMap_[info_.queue[executeIndex_].funcName](this, info_.queue[executeIndex_].args);
		++executeIndex_;
	}

	if (wating_) {
		Wait(info_.queue[executeIndex_].args);
	}

	ExecuteQueue();
}

void Enemy::OnCollision(Object* other) {
}

void Enemy::AddFireDesc(std::vector<std::shared_ptr<Value>> args) {
	int cooltime = 30;

	EnemyBulletDesc desc{};
	desc.position = transform_->position;
	for (auto& arg : args) {
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

		} else if (arg->name == "isTracking") {
			desc.isTracking = dynamic_cast<IntValue*>(arg.get())->value != 0;
		} else if (arg->name == "cooltime") {
			cooltime = dynamic_cast<IntValue*>(arg.get())->value;
		}
	}

}

void Enemy::Wait(std::vector<std::shared_ptr<Value>> args) {
	//waitの初期化処理
	if (!wating_) {

		wating_ = true;

		if (args.size() <= 0) {
			waitTime_ = 60;
		} else {
			waitTime_ = dynamic_cast<IntValue*>(args[0].get())->value;
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

void Enemy::Accel(std::vector<std::shared_ptr<Value>> args) {
	EnqueueAction(std::make_shared<AccelAct>(this, args));
}

AccelAct::AccelAct(Enemy* enemy, std::vector<std::shared_ptr<Value>> args) : Action("Accel", enemy) {

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

	enemy_ = enemy;
	isAccelerating_ = true;
}

void AccelAct::Execute() {
	Vector3 acceleration = acceleration_;
	addedVelocity_ += acceleration;

	int limited = 0;			//加速制限をかけられた回数

	for (int i = 0; i < 3; ++i) {
		if (targetVelocity_[i] < addedVelocity_[i]) {
			acceleration[i] -= addedVelocity_[i] - targetVelocity_[i];
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
