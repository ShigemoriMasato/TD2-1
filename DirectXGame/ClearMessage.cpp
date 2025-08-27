#include "ClearMessage.h"
#include <numbers>

ClearMessageObj::ClearMessageObj(int handle, Camera* camera) : Object(camera, ShapeType::Model) {
	handle_ = handle;
	transform_->rotation.x = -std::numbers::pi_v<float> / 2.0f;
}

ClearMessageObj::~ClearMessageObj() {
}

void ClearMessageObj::Initialize() {
	isRotate_ = true;
	frame_ = 0;
}

void ClearMessageObj::Update(Vector3 pos) {
	frame_++;

	if (isRotate_) {
		float theta = frame_ / 40.0f;
		float t = std::sin(theta);

		transform_->rotation.y = t * std::numbers::pi_v<float>;

		if (theta >= std::numbers::pi_v<float> / 2.0f) {
			isRotate_ = false;
			transform_->rotation.y = std::numbers::pi_v<float>;
		}

	} else {
		transform_->rotation.y = std::numbers::pi_v<float>;
	}

	float theta = frame_ / 30.0f;
	transform_->position = pos + Vector3(0, std::sin(theta) * 0.2f, 0);
}

ClearMessage::ClearMessage(CommonData* cd) {
	camera_ = std::make_shared<Camera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->MakeMatrix();

	for (int i = int(ModelType::C); i <= int(ModelType::Bikkuri); ++i) {
		auto obj = std::make_unique<ClearMessageObj>(cd->modelHandle_[i], camera_.get());
		obj->Initialize();
		clearMessageObjs_.push_back(std::move(obj));
	}

	positions_.resize(clearMessageObjs_.size());

	binaryManager_ = std::make_unique<BinaryManager>();
}

ClearMessage::~ClearMessage() {
}

void ClearMessage::Initialize() {
	auto data = binaryManager_->Read(fileName_);

	for (int i = 0; i < data.size(); ++i) {
		positions_[i] = dynamic_cast<Value<Vector3>*>(data[i].get())->value;
	}

	for (auto& obj : clearMessageObjs_) {
		obj->Initialize();
	}

	frame_ = 0;
}

void ClearMessage::Update() {
	++frame_;

	ImGui::Begin("ClearMessage");
	ImGui::DragFloat3("C", &positions_[0].x, 0.01f);
	ImGui::DragFloat3("L", &positions_[1].x, 0.01f);
	ImGui::DragFloat3("E", &positions_[2].x, 0.01f);
	ImGui::DragFloat3("A", &positions_[3].x, 0.01f);
	ImGui::DragFloat3("R", &positions_[4].x, 0.01f);
	ImGui::DragFloat3("!", &positions_[5].x, 0.01f);

	if(ImGui::Button("Save")) {
		for(auto& pos : positions_) {
			binaryManager_->RegistOutput(pos, "p");
		}
		binaryManager_->Write(fileName_);
	}

	ImGui::End();

	for(int i = 0; i < clearMessageObjs_.size(); ++i) {
		if (frame_ >= 10 * i) {
			clearMessageObjs_[i]->Update(positions_[i]);
		}
	}
}

void ClearMessage::Draw() {
	for (int i = 0; i < clearMessageObjs_.size(); ++i) {
		if (frame_ >= 10 * i) {
			clearMessageObjs_[i]->Draw();
		}
	}
}
