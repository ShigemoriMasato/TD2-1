#include "Player.h"
#include "Wire.h"

namespace {
	//debug用
	const float wireTime = 0.5f;
	float wireTimer = 0.0f;
}

void Player::OnIdel() {
}

void Player::UpdateIdel(float deltaTime) {
	//初期化
	auto key = (*key_);

	//移動
	if (key[Key::Right]) actor_->velocity_.x += moveSpeed_;
	if (key[Key::Left]) actor_->velocity_.x -= moveSpeed_;

	//Behaviorリクエスト
	if (key[Key::Action]) {
		behaviorRequest_ = Behavior::Forcus;
	}
}

void Player::OnForcus() {
	timeSlower_->StartSlow(3.0f);
}

void Player::UpdateForcus(float deltaTime) {
	//todo 狙い先の当たり判定をとる。
	//todo 当たり判定の具体的な値の送信方法は後日相談

	auto key = (*key_);
	if (!key[Key::Action]) behaviorRequest_ = Behavior::Extend;


	//以下仮置き
	int directionID = -1;
	
	//右側
	if (key[Key::Right]) {

		if (key[Key::Up]) {
			directionID = 1;
		} else if(key[Key::Down]){
			directionID = 7;
		} else {
			directionID = 0;
		}

	} 
	//左側
	else if(key[Key::Left]) {

		if (key[Key::Up]) {
			directionID = 3;
		}else if(key[Key::Down]) {
			directionID = 5;
		} else {
			directionID = 4;
		}

	} 
	//上下
	else {
		if (key[Key::Up]) {
			directionID = 2;
		} else if (key[Key::Down]) {
			directionID = 6;
		}
	}

}

void Player::OnExtend() {
	actor_->velocity_ = {};
	//スロウモーション終了
	timeSlower_->EndSlow(false);
	//重力無効化
	actor_->useGravity_ = false;

	//debug
	wireTimer = wireTime;
}

void Player::UpdateExtend(float deltaTime) {
	//? Wireを伸ばすアップデート	debug
	wireTimer -= deltaTime;

	//? Wireが届いたらDashへ		debug
	if (wireTimer <= 0.0f) {
		behaviorRequest_ = Behavior::Dash;
	}
}

void Player::OnDash() {

	//if (!wire_->IsHited()) {
	//	return;
	//}

	//actor_->velocity_ = wire_->GetDirection() * dashPower_;
	
	//↓仮置き(斜め45度くらいで吹っ飛ばす)
	actor_->velocity_ = Vector3(0.71f, 0.71f, 0.0f) * dashPower_;
	if (transform_.position.y == 0.0f) {
		transform_.position.y = 0.01f;
	}

	actor_->useGravity_ = true;
}

void Player::UpdateDash(float deltaTime) {
	actor_->velocity_ *= dashRegistRate_;

	//if(着地したら){
	//	behaviorRequest_ = Behavior::Idel;
	//}

	auto key = (*key_);

	//空中でワイヤーを伸ばせるようにする
	if (key[Key::Action]) {
		behaviorRequest_ = Behavior::Forcus;
	}

	//velocityの微調整をできるようにする
	if (key[Key::Right]) actor_->velocity_.x += dashMoveSpeed_ * deltaTime;
	if (key[Key::Left]) actor_->velocity_.x -= dashMoveSpeed_ * deltaTime;

	if (transform_.position.y <= 0.0f) {
		transform_.position.y = 0.0f;
		behaviorRequest_ = Behavior::Idle;
	}
}
