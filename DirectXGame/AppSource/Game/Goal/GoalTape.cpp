#include "GoalTape.h"

GoalTape::GoalTape() {
	drawResource_ = std::make_unique<DrawResource>();
	drawResource_->Initialize(ShapeType::Plane);
}

GoalTape::~GoalTape() {
}

void GoalTape::Initialize(TextureManager* textureManager, float positionX, float scaleY, PhysicsEngine* engine, Camera* camera) {
	int textureHandle = textureManager->LoadTexture("Assets/Texture/goal.png");
	drawResource_->textureHandle_ = textureHandle;

	transform_.scale = { 1.0f, 40.0f, 1.0f };
	transform_.position = { positionX, 10.0f, 0.0f };
	drawResource_->position_ = transform_.position;
	drawResource_->scale_ = transform_.scale;
	drawResource_->camera_ = camera;
	drawResource_->color_ = 0xffffffff;
	drawResource_->texcoord_ = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, transform_.scale.y / 10.0f},
		{1.0f, transform_.scale.y / 10.0f},
	};

	actor_ = std::make_unique<PhysicsActor>(engine, this);
	actor_->useGravity_ = false;

	collider_ = std::make_unique<AABBCollider>(ColliderTag::Static, ColliderMask::ITEM, ColliderMask::PLAYER, Vector3(1,1,0.01f));
	collider_->SetTransform(&transform_);
}

void GoalTape::Update(float deltaTime) {
	drawResource_->texturePos_.y -= deltaTime / 2;
}

void GoalTape::Draw(Render* render) {
	render->Draw(drawResource_.get());
}

void GoalTape::OnCollision(BaseObject* other) {
	if (other->GetCollider()->GetSelf() == ColliderMask::PLAYER) {
		isClear_ = true;
	}
}
