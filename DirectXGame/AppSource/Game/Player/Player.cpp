#include "Player.h"

Player::Player() {
}

Player::~Player() {
}

void Player::Initialize(ModelData* modelData) {
	//モデルリソースの作成
	modelResource_ = std::make_unique<ModelResource>();
	modelResource_->Initialize(modelData);
}

void Player::Update() {
	//更新
}

void Player::Draw(Render* render) {
	//描画
	render->Draw(modelResource_.get());
}
