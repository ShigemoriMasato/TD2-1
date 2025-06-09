#include "Player.h"
#include "Engine/Render/Render.h"

using namespace Matrix;

void Player::Initialize(Camera* camera) {
	camera_ = camera;
	transform_ = Transform();
}

void Player::Update() {
}

void Player::Draw() const {
	Render::DrawBox(MakeAffineMatrix(transform_), camera_, { 1.0f, 1.0f, 1.0f, 1.0f, true }, {}, 1);
}
