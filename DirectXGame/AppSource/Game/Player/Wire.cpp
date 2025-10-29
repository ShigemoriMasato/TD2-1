#include "Wire.h"

void Wire::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData, camera);
	drawResource_ = std::make_unique<DrawResource>();
	drawResource_->Initialize(2);
	drawResource_->camera_ = camera;
	drawResource_->psoConfig_.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	collider_ = std::make_unique<SphereCollider>(
		ColliderTag::Dynamic, 
		ColliderMask::WIRE,
		ColliderMask::ENEMY | ColliderMask::GOAL, 
		5.0f);
	collider_->SetTransform(&transform_);
}

void Wire::Update(float deltaTime) {
	preEndPos_ = endPos_;
	transform_.position = *startPos_;
	if (isExtending_) {

		Vector3 direction = (targetPos_ - *startPos_).Normalize();
		float currentDistance = (endPos_ - *startPos_).Length();
		float totalDistance = (targetPos_ - *startPos_).Length();
		//現在距離から進ませる
		currentDistance += extendSpeed_ * deltaTime;


		if (currentDistance >= totalDistance - 0.001f)
		{
			endPos_ = targetPos_;
			isExtending_ = false;
		}
		else
		{
			endPos_ = *startPos_ + direction * currentDistance;
		}
		if ((endPos_ - *startPos_).Length() < 1.0f)
		{
			isExtending_ = false;
			endPos_ = targetPos_;
		}

	} else {
		if (isVisible_ && endPos_ != targetPos_) {
			isVisible_ = true;
		}
	}

	if (!isVisible_) {
		endPos_ = *startPos_;
	}

#ifdef _DEBUG
	ImGui::Begin("Wire");
	ImGui::Text("startPos x:%f y%f", startPos_->x, startPos_->y);
	ImGui::Text("endPos x:%f y%f", endPos_.x, endPos_.y);
	ImGui::Text("isExtending:%d", isExtending_);
	ImGui::End();
#endif
}

void Wire::Draw(Render* render) {
	drawResource_->localPos_[0] = {*startPos_};
	drawResource_->localPos_[1] = { endPos_ };

	render->Draw(drawResource_.get());
}

void Wire::SetEndPosition(const Vector3& endPos) {
	//目標座標を設定
	targetPos_ = endPos;
	//startからendまでの距離
	initDistance_ = (endPos - *startPos_).Length();
	//伸ばし始める
	isExtending_ = true;
	//可視化させる
	isVisible_ = true;
}

bool Wire::Extended() {
	return !isExtending_;
}

void Wire::Shrinked() {
	isVisible_ = false;
}
