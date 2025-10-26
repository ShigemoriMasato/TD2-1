#include "GoalTape.h"

GoalTape::GoalTape() {
	drawResource_ = std::make_unique<DrawResource>();
	drawResource_->Initialize(ShapeType::Plane);
}

GoalTape::~GoalTape() {
}

void GoalTape::Initialize(int textureHandle, float positionX, float scaleY, PhysicsEngine* engine, Camera* camera) {
	drawResource_->SetTextureHandle(textureHandle);

	transform_.scale = { 1.0f, scaleY, 1.0f };
	transform_.position = { positionX, scaleY / 2.0f - 0.5f, 0.0f };
	drawResource_->position_ = transform_.position;
	drawResource_->scale_ = transform_.scale;
	drawResource_->camera_ = camera;
	drawResource_->color_ = 0xffffffff;
	drawResource_->texcoord_ = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, scaleY / 10.0f},
		{1.0f, scaleY / 10.0f},
	};

	collider_ = std::make_unique<AABBCollider>(ColliderTag::Static, ColliderMask::GOAL, ColliderMask::GOAL, Vector3(1,1,0.01f));
	collider_->SetTransform(&transform_);
}

void GoalTape::Update(float deltaTime) {
	drawResource_->texturePos_.y -= deltaTime / 2;
}

void GoalTape::Draw(Render* render) {
	render->Draw(drawResource_.get());
}
