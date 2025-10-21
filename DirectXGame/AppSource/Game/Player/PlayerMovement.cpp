#include "Player.h"
#include "Wire.h"
#include "../Tile/TileMap.h"

namespace {
	//debug用
	const float wireTime = 0.5f;
	float wireTimer = 0.0f;
}

void Player::OnIdel() {

	actor_->useGravity_ = true;
	wire_->SetStartPositionPtr(&transform_.position);
	wire_->Shrinked();
}

void Player::UpdateIdel(float deltaTime) {
	//初期化

	auto& key = (*key_);

	//移動
	if (actor_->collidedBottom_)
	{
		if (key[Key::Right]) actor_->force_.x = moveSpeed_;
		if (key[Key::Left]) actor_->force_.x = -moveSpeed_;
	}

	//Behaviorリクエスト
	if (key[Key::Action]) {
		behaviorRequest_ = Behavior::Forcus;
	}
}

void Player::OnForcus() {
	timeSlower_->StartSlow(3.0f);
}

void Player::UpdateForcus(float deltaTime) {
	//地面に着地していたら慣性を消す
	if(actor_->collidedBottom_) {
		actor_->velocity_ = {};
	}

	//todo 狙い先の当たり判定をとる。
	//todo 当たり判定の具体的な値の送信方法は後日相談

	auto& key = (*key_);
	if ((targets_.empty() || actor_->collidedBottom_) && !key[Key::Action])
	{
		timeSlower_->EndSlow(true);
		behavior_ = Behavior::Idle;
	}
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


	if (!key[Key::Action]) { 
		//ワイヤーの範囲に入ったオブジェクトチェック
		Vector3 inputDirection = GetInputDirection();
		if (inputDirection.Length() > 0.1f)
		{
			BaseObject* selectedTarget = SelectTargetByDirection(inputDirection);

			if (selectedTarget)
			{
				targetPos_ = selectedTarget->GetTransform()->position;
				if (!tileMap_->HasTile(this->transform_.position, targetPos_, TileType::Solid))
				{
					wire_->SetEndPosition(targetPos_);
					behaviorRequest_ = Behavior::Extend;
					//todo:スローでオブジェクト選択できるようになる
				}
			}
		}
	}

}

void Player::OnExtend() {
	actor_->velocity_ = {};
	actor_->useGravity_ = false;
	wireTimer = wireTime;
	timeSlower_->EndSlow(false);
}

void Player::UpdateExtend(float deltaTime) {
	//やんわり落下させる(/ 10.0fはやんわりのために雑に決めただけ)
	actor_->force_.y = extendGravityRate_;

	//Wireが届いたらDashへ
	if (wire_->Extended()) {
		behaviorRequest_ = Behavior::Shrink;
	}
}

void Player::OnShrink() {
	targetDir_ = (targetPos_ - transform_.position).Normalize();
	actor_->velocity_ = targetDir_ * dashPower_;
}

void Player::UpdateShrink(float deltaTime) {
	//velocityの固定
	actor_->velocity_ = targetDir_ * dashPower_;

	float tarlen = targetPos_.Length();
	float plalen = transform_.position.Length();
	//playerがtarlenに一定以上近くなったらダッシュに切り替え
	if (plalen > tarlen - 0.5f && plalen < tarlen + 0.5f) {
		wire_->Shrinked();
		behaviorRequest_ = Behavior::Dash;
	}
}

void Player::OnDash() {
	//↓仮置き(斜め45度くらいで吹っ飛ばす)
	actor_->force_ = targetDir_ * dashPower_;

	actor_->useGravity_ = true;
}

void Player::UpdateDash(float deltaTime) {
	actor_->velocity_ *= dashRegistRate_;

	if(actor_->collidedBottom_){
		behaviorRequest_ = Behavior::Idle;
	}

	auto& key = (*key_);

	//空中でワイヤーを伸ばせるようにする
	if (key[Key::Action]) {
		behaviorRequest_ = Behavior::Forcus;
	}

	//velocityの微調整をできるようにする
	if (key[Key::Right]) actor_->velocity_.x += dashMoveSpeed_ * deltaTime;
	if (key[Key::Left]) actor_->velocity_.x -= dashMoveSpeed_ * deltaTime;

	if (actor_->collidedBottom_) {
		behaviorRequest_ = Behavior::Idle;
	}
}
