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
	int totalSeconds = std::max(0, number);

	// 秒数を分と秒に分解
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	
	int displayValue = 0;
	if (specifiedDigitCount_ == 4) {
		// MM:SS形式 (分2桁+秒2桁)
		minutes = std::min(minutes, 99);  // 最大99分
		displayValue = minutes * 100 + seconds;

	} else if (specifiedDigitCount_ == 6) {

		// MMM:SS形式 (分3桁+秒2桁) または HH:MM:SS
		// ここではMMM:SS形式として扱う
		minutes = std::min(minutes, 999);  // 最大999分
		displayValue = minutes * 100 + seconds;

	} else {

		// その他の桁数の場合は従来通り数値をそのまま表示
		displayValue = totalSeconds;
	}

	displayNumber_ = displayValue;

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

		// 区切り文字の数を計算（この桁より右にある区切り文字の数）
		int separatorCountBefore = 0;
		for (int pos : separatorPositions_) {
			if (pos <= i) {
				separatorCountBefore++;
			}
		}

		// 位置を設定（右から左に配置、区切り文字分のオフセットを加える）
		Vector3 digitPosition = position_;
		// 数字間の間隔を広げる（scale_.x * 1.3倍）
		digitPosition.x -= scale_.x * 1.3f * (i + separatorCountBefore * 0.5f);
		resource->position_ = digitPosition;

		// カメラと色を設定
		resource->camera_ = camera_;
		resource->scale_ = scale_;
		resource->color_ = color_;
	}

	// 区切り文字の更新
	for (size_t i = 0; i < separatorResources_.size(); ++i) {
		DrawResource* separator = separatorResources_[i].get();
		
		// i番目の区切り文字の位置を取得
		auto it = separatorPositions_.begin();
		std::advance(it, i);
		int position = *it;

		// この区切り文字より右にある区切り文字の数
		int separatorCountBefore = static_cast<int>(i);

		// 位置を設定（桁と桁の中間に配置）
		Vector3 separatorPosition = position_;
		// 数字間隔に合わせてコロンの位置も調整
		separatorPosition.x -= scale_.x * 1.3f * position + scale_.x * 1.3f * separatorCountBefore * 0.5f - scale_.x * 0.35f;
		separator->position_ = separatorPosition;

		// カメラと色を設定
		separator->camera_ = camera_;
		separator->scale_ = scale_;
		separator->color_ = color_;
	}
}

void TimerResource::SetSeparator(const std::set<int>& positions, const std::string& separatorTexturePath) {
	separatorPositions_ = positions;
	separatorTexturePath_ = separatorTexturePath;

	// 区切り文字用のDrawResourceを作成
	separatorResources_.clear();
	separatorResources_.resize(positions.size());

	for (size_t i = 0; i < separatorResources_.size(); ++i) {
		separatorResources_[i] = std::make_unique<DrawResource>();
		separatorResources_[i]->Initialize(ShapeType::Plane);
		separatorResources_[i]->scale_ = scale_;
		separatorResources_[i]->color_ = color_;
		separatorResources_[i]->SetTextureHandle(separatorTexturePath_);
	}
}

void TimerResource::ClearSeparators() {
	separatorPositions_.clear();
	separatorResources_.clear();
	separatorTexturePath_.clear();
}

std::vector<DrawResource*> TimerResource::GetDrawResources() const {
	std::vector<DrawResource*> resources;
	resources.reserve(specifiedDigitCount_ + separatorResources_.size());

	// 指定された桁数分すべて返す
	for (int i = 0; i < specifiedDigitCount_; ++i) {
		resources.push_back(digitResources_[i].get());
	}

	// 区切り文字も追加
	for (const auto& separator : separatorResources_) {
		resources.push_back(separator.get());
	}

	return resources;
}
