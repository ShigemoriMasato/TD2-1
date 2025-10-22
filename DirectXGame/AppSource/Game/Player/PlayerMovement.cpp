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

	if (key[Key::Right]) actor_->force_.x = moveSpeed_;
	if (key[Key::Left]) actor_->force_.x = -moveSpeed_;


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

	auto& key = (*key_);

	//狙い先の当たり判定をとる。
	//ワイヤーの範囲に入ったオブジェクトチェック
	Vector3 inputDirection = GetInputDirection();
	BaseObject* selectedTarget = nullptr;

	if (inputDirection.Length() > 0.1f) {
		selectedTarget = SelectTargetByDirection(inputDirection);

		if (selectedTarget) {
			targetPos_ = selectedTarget->GetTransform()->position;
		}
	}

	if (!key[Key::Action]) {

		//ターゲットが選ばれていたらワイヤーを伸ばす
		if (selectedTarget) {

			//ワイヤーの終端をセット
			wire_->SetEndPosition(targetPos_);
			//スロウモーション終了(補完なし)
			timeSlower_->EndSlow(false);
			//ワイヤーを投げる
			behaviorRequest_ = Behavior::Extend;

		} else {

			//スロウモーション終了(補完あり)
			timeSlower_->EndSlow(true);
			//着地したらIdleに行くはずなのでとりあえずDashに投げる
			behaviorRequest_ = Behavior::Dash;

		}

	}

}

void Player::OnExtend() {
	actor_->velocity_ = {};
	actor_->useGravity_ = false;
	wireTimer = wireTime;
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
}

void Player::UpdateShrink(float deltaTime) {
	//加速
	dashPower_ += dashAcceleration_ * deltaTime;
	dashPower_ = std::min(dashPower_, maxDashSpeed_);
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

	//if(着地したら){
	//	behaviorRequest_ = Behavior::Idel;
	//}

	auto& key = (*key_);

	//空中でワイヤーを伸ばせるようにする
	if (key[Key::Action]) {
		behaviorRequest_ = Behavior::Forcus;
	}

	//velocityの微調整をできるようにする
	if (key[Key::Right]) actor_->velocity_.x += dashMoveSpeed_ * deltaTime;
	if (key[Key::Left]) actor_->velocity_.x -= dashMoveSpeed_ * deltaTime;

	//地面についたら
	if (actor_->collidedBottom_) {
		behaviorRequest_ = Behavior::Idle;
	}
}
