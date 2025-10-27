#include "Player.h"
#include "Wire.h"
#include "../Tile/TileMap.h"
#include <Resource/Audio/AudioManager.h>

void Player::OnIdel() {
	transform_.rotation = {};
	rotateSpeed_ = 0.0f;

	//現在のスロウタイムをリセット
	currentSlowTime_ = maxSlowTime_;

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

	//音
	if(key[Key::Right] || key[Key::Left]) {
		walkSeTimer_ += deltaTime;
		if (walkSeTimer_ >= walkCoolTime_) {
			int walk = audio_->Load("walk.mp3");
			audio_->Play(walk, false);
			walkSeTimer_ = 0.0f;
		}
	} else {
		walkSeTimer_ = walkCoolTime_;
	}

	if(!actor_->collidedBottom_){
		behaviorRequest_ = Behavior::Dash;
	}

	//Behaviorリクエスト
	if (key[Key::Action]) {
		behaviorRequest_ = Behavior::Forcus;
	}
}

void Player::OnForcus() {
	int slow = audio_->Load("slow.mp3");
	audio_->Play(slow, false);
	timeSlower_->StartSlow(currentSlowTime_);
}

void Player::UpdateForcus(float deltaTime) {
	//地面に着地していたら慣性を消す
	if (actor_->collidedBottom_) {
		actor_->velocity_ = {};
	}
	actor_->velocity_ *= dashRegistRate_;

	currentSlowTime_ = std::max(currentSlowTime_ - deltaTime, 0.0f);

	auto& key = (*key_);

	//狙い先の当たり判定をとる。
	//ワイヤーの範囲に入ったオブジェクトチェック
	Vector3 inputDirection = GetInputDirection();
	BaseObject* selectedTarget = nullptr;

	targetPos_ = {};

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
			//現在のスロウタイムをリセット
			currentSlowTime_ = maxSlowTime_;
			//ワイヤーを投げる
			behaviorRequest_ = Behavior::Extend;
			selectedTarget->SetColor(0xffffffff);
		} else {

			//スロウモーション終了(補完あり)
			timeSlower_->EndSlow(true);
			//着地したらIdleに行くはずなのでとりあえずDashに投げる
			behaviorRequest_ = Behavior::Dash;
		}
	}

}

void Player::OnExtend() {

	int shoot = audio_->Load("shoot.mp3");
	audio_->Play(shoot, false);

	actor_->velocity_ = {};
	actor_->useGravity_ = false;
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

	int reel = audio_->Load("reel.mp3");
	audio_->Play(reel, false);

	targetDir_ = (targetPos_ - transform_.position).Normalize();
	rotateSpeed_ = rotateMaxSpeed_;
}

void Player::UpdateShrink(float deltaTime) {
	//加速
	dashPower_ += dashAcceleration_ * deltaTime;
	dashPower_ = std::min(dashPower_, maxDashSpeed_);
	//velocityの固定
	actor_->velocity_ = targetDir_ * dashPower_;

	float tarlen = targetPos_.Length();
	float plalen = transform_.position.Length();
	//playerがtarlenに一定以上近くなったり、スペースを押したらダッシュに切り替え
	if ((plalen > tarlen - 0.5f && plalen < tarlen + 0.5f) || (*key_)[Key::Action]) {
		wire_->Shrinked();
		behaviorRequest_ = Behavior::Dash;
	}
}

void Player::OnDash() {
	actor_->useGravity_ = true;
}

void Player::UpdateDash(float deltaTime) {
	actor_->velocity_ *= dashRegistRate_;

	auto& key = (*key_);

	//空中でワイヤーを伸ばせるようにする
	if (key[Key::Action]) {
		behaviorRequest_ = Behavior::Forcus;
	}

	//velocityの微調整をできるようにする
	if (key[Key::Right]) actor_->force_.x = dashMoveSpeed_ * deltaTime;
	if (key[Key::Left]) actor_->force_.x = -dashMoveSpeed_ * deltaTime;

	//地面についたら
	if (actor_->collidedBottom_) {
		behaviorRequest_ = Behavior::Idle;
	}
}

void Player::OnClear() {
	rotateSpeed_ = rotateMaxSpeed_ / 2.0f;
}

void Player::UpdateClear(float deltaTime) {
	actor_->velocity_ = { 5.0f, 0.0f };
}
