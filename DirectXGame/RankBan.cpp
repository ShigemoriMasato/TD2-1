#include "RankBan.h"
#include <Input/Input.h>
#include <Data/UniqueNumber.h>

std::array<int, 10> Number::handles_ = {};
float Number::ratio_ = {};
Vector2 Number::size_ = { 47, 71 };

Number::Number(Camera* camera) : Object(camera, ShapeType::Sprite) {
	SetLocalPosition(
		{ -size_.x / 2, size_.y / 2, 0.0f },
		{ size_.x / 2, size_.y / 2, 0.0f },
		{ -size_.x / 2, -size_.y / 2, 0.0f },
		{ size_.x / 2, -size_.y / 2, 0.0f }
	);
	handle_ = handles_[0];
}

void Number::SetPosition(Vector3 pos) {
	transform_->position = pos;
}

void Number::Update() {
	transform_->scale = { ratio_, ratio_, 1.0f };
}

RankBan::RankBan(CommonData* cd) {
	std::array<int, 10> handles;
	for (int i = 0; i < 10; ++i) {
		handles[i] = cd->textureHandle_[int(TextureType::t0) + i];
	}

	Number::SetHandles(handles);
	manager_ = std::make_unique<BinaryManager>();
}

void RankBan::Initialize() {
	{
		auto data = manager_->Read("Ranking.dat");

		for (auto& d : data) {
			times_.push_back(dynamic_cast<Value<double>*>(d.get())->value);
		}
	}

	{
		auto data = manager_->Read("RankingConfig.dat");

		if (data.size() > 0) {
			for (int i = 0; i < int(data.size() - 1); ++i) {
				positions_.push_back(dynamic_cast<Value<Vector3>*>(data[i].get())->value);
			}

			ratio_ = dynamic_cast<Value<float>*>(data.back().get())->value;
			Number::SetRatio(ratio_);
		}

		positions_.resize(9);
	}

	camera_ = std::make_shared<Camera>();
	camera_->SetProjectionMatrix(OrthographicDesc());
	camera_->MakeMatrix();
	
	//(何位かを表す数字[1] + 秒数[2] + コンマ何秒[3]) * 9個分
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 6; j++) {
			numbers_[i][j] = std::make_unique<Number>(camera_.get());
		}
	}
}

void RankBan::Update() {
	//特定キーによるランキングの初期化
	if (Input::GetKeyState(DIK_F8)) {
		for (auto& time : times_) {
			time = UniqueNumber::Double::max;
			manager_->RegistOutput(time, "t");
		}
		manager_->Write("Ranking.dat");
	}

	// imguiによる大きさ等の調整
	ImGui::Begin("RankingConfig");

	for (int i = 0; i < 9; ++i) {
		ImGui::DragFloat3(("Position" + std::to_string(i + 1)).c_str(), &positions_[i].x, 1.0f, -1280.0f, 720.0f);
	}

	ImGui::DragFloat("Ratio", &ratio_, 0.01f, 0.1f, 5.0f);
	Number::SetRatio(ratio_);

	if(ImGui::Button("Save")) {
		for (int i = 0; i < 9; ++i) {
			manager_->RegistOutput(positions_[i], "v");
		}
		manager_->RegistOutput(ratio_, "f");
		manager_->Write("RankingConfig.dat");
	}

	ImGui::End();

	//ランキングの数字のポジション管理
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			int index = i * 3 + j;

			numbers_[index][0]->SetPosition(positions_[index]);

			for (int k = 1; k < 3; ++k) {
				numbers_[index][k]->SetPosition({ (positions_[index].x + 47 * k + 20) * ratio_, positions_[index].y, positions_[index].z });
			}

			for(int k = 3; k < 6; ++k) {
				numbers_[index][k]->SetPosition({ (positions_[index].x + 47 * k + 40) * ratio_, positions_[index].y, positions_[index].z });
			}

		}
	}

	for(auto& row : numbers_) {
		for(auto& num : row) {
			num->Update();
		}
	}
}

void RankBan::Draw() {
	for(auto& row : numbers_) {
		for(auto& num : row) {
			num->Draw();
		}
	}
}
