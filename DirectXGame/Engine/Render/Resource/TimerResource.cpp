#include "TimerResource.h"
#include <algorithm>
#include <cmath>

void TimerResource::Initialize(const Vector3& scale, int digitCount) {
	scale_ = scale;
	specifiedDigitCount_ = std::max(1, digitCount);  // 最低1桁は保証

	// 指定された桁数分のDrawResourceを作成
	digitResources_.resize(specifiedDigitCount_);
	for (int i = 0; i < specifiedDigitCount_; ++i) {
		digitResources_[i] = std::make_unique<DrawResource>();
		digitResources_[i]->Initialize(ShapeType::Plane);
		digitResources_[i]->scale_ = scale_;
		digitResources_[i]->color_ = color_;
	}
}

void TimerResource::Update(int number) {
	// 負の数は0として扱う
	displayNumber_ = std::max(0, number);

	// 指定桁数を超える場合は切り詰める（最大値に制限）
	int maxValue = static_cast<int>(std::pow(10, specifiedDigitCount_)) - 1;
	displayNumber_ = std::min(displayNumber_, maxValue);

	// 各桁を抽出（0埋めするため指定桁数分すべて処理）
	std::vector<int> digits(specifiedDigitCount_, 0);
	int tempNumber = displayNumber_;

	// 右から順に桁を抽出
	for (int i = 0; i < specifiedDigitCount_; ++i) {
		digits[i] = tempNumber % 10;
		tempNumber /= 10;
	}

	// 各桁のDrawResourceを設定
	for (int i = 0; i < specifiedDigitCount_; ++i) {
		int digit = digits[i];
		DrawResource* resource = digitResources_[i].get();

		// テクスチャを設定（numberフォルダのパス）
		std::string texturePath = "Assets/Texture/number/" + std::to_string(digit) + ".png";
		resource->SetTextureHandle(texturePath);

		// 位置を設定（右から左に配置）
		Vector3 digitPosition = position_;
		digitPosition.x -= scale_.x * i;  // 右詰めなので左に移動
		resource->position_ = digitPosition;

		// カメラと色を設定
		resource->camera_ = camera_;
		resource->scale_ = scale_;
		resource->color_ = color_;
	}
}

std::vector<DrawResource*> TimerResource::GetDrawResources() const {
	std::vector<DrawResource*> resources;
	resources.reserve(specifiedDigitCount_);

	// 指定された桁数分すべて返す
	for (int i = 0; i < specifiedDigitCount_; ++i) {
		resources.push_back(digitResources_[i].get());
	}

	return resources;
}
