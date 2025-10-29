#include "NumberPlate.h"

void NumberPlate::Initialize(TextureManager* textureManager, int digit, bool isTimer) {
	for (int i = 0; i < 10; ++i) {
		textureHandles_.push_back(textureManager->LoadTexture("Assets/Texture/number/" + std::to_string(i) + ".png"));
	}

	digit_ = digit;
	isTimer_ = isTimer;

	camera_ = std::make_unique<Camera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->MakeMatrix();

	for (int i = 0; i < digit; ++i) {
		res_.emplace_back(std::make_unique<DrawResource>());
		res_[i]->Initialize(ShapeType::Plane);
		res_[i]->camera_ = camera_.get();
		res_[i]->SetTextureHandle(textureHandles_[0]);
	}

	if (isTimer) {
		res_.emplace_back(std::make_unique<DrawResource>());
		res_.back()->Initialize(ShapeType::Plane);
		res_.back()->camera_ = camera_.get();
		res_.back()->SetTextureHandle(textureManager->LoadTexture("Assets/Texture/number/colon.png"));
	}

	res_.emplace_back(std::make_unique<DrawResource>());
	res_.back()->Initialize(ShapeType::Plane);
	res_.back()->camera_ = camera_.get();
	res_.back()->SetTextureHandle(textureManager->LoadTexture("Assets/Texture/number/minas.png"));

}

void NumberPlate::Update(int num) {
	std::vector<int> digits;

	int absNum = std::abs(num);

	if (isTimer_) {

		int sec = absNum % 60;
		digits.push_back(sec % 10);
		digits.push_back(sec / 10);
		int minutes = absNum / 60;
		for (int i = 2; i < digit_; ++i) {
			digits.push_back(minutes % 10);
			minutes /= 10;
		}

	} else {

		for (int i = 0; i < digit_; ++i) {
			digits.push_back(absNum % 10);
			absNum /= 10;
		}

	}

	for (int i = 0; i < digit_; ++i) {
		res_[i]->position_ = transform_.position;
		
		if (isTimer_) {
			res_[i]->position_.x -= (i * transform_.scale.x) + float(i > 1) * 0.8f * transform_.scale.x;
		} else {
			res_[i]->position_.x -= i * transform_.scale.x;
		}

		res_[i]->rotate_ = transform_.rotation;
		res_[i]->scale_ = transform_.scale;
		if (i < digits.size()) {
			res_[i]->SetTextureHandle(textureHandles_[digits[i]]);
		} else {
			res_[i]->SetTextureHandle(textureHandles_[0]);
		}
	}

	if (isTimer_) {
		static float buff;
		ImGui::Begin("a");
		ImGui::DragFloat("offset", &buff, 0.01f);
		ImGui::End();

		res_[(int)res_.size() - 1]->position_ = transform_.position;
		res_[(int)res_.size() - 1]->position_.x -= transform_.scale.x + 0.8f * transform_.scale.x;
		res_[(int)res_.size() - 1]->rotate_ = transform_.rotation;
		res_[(int)res_.size() - 1]->scale_ = transform_.scale;
	}

	//マイナス表示
	if (num < 0) {
		res_.back()->position_ = transform_.position;
		if (isTimer_) {
			res_.back()->position_.x -= (digit_ * transform_.scale.x) + 0.8f * transform_.scale.x;
		} else {
			res_.back()->position_.x -= digit_ * transform_.scale.x;
		}
		res_.back()->rotate_ = transform_.rotation;
		res_.back()->scale_ = transform_.scale;
		res_.back()->color_ = 0xffffffff;
	} else {
		res_.back()->color_ = 0x00000000;
	}
}

void NumberPlate::Draw(Render* render) {
	for (int i = 0; i < res_.size(); ++i) {
		render->Draw(res_[i].get());
	}
}

void NumberPlate::SetPosition(Vector3 position) {
	transform_.position = position;
}
