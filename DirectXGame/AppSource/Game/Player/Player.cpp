#include "Player.h"

Player::Player() {
}

Player::~Player() {
}

void Player::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData,camera);

	modelResource_->psoConfig_.isSwapChain = true;
}

void Player::Update() {
	//更新


}

