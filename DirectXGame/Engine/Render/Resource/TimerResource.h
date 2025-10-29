#pragma once
#include "DrawResource.h"
#include <vector>
#include <memory>
#include <set>

/// <summary>
/// 数字テクスチャを使用して右詰めで数字を表示するタイマークラス
/// </summary>
class TimerResource {
public:
	TimerResource() = default;
	~TimerResource() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="scale">表示するスケール</param>
	/// <param name="digitCount">表示する桁数</param>
	void Initialize(const Vector3& scale, int digitCount, bool isTimer);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="number">表示する数字</param>
	void Update(int number);

	/// <summary>
	/// 描画用のDrawResourceを取得
	/// </summary>
	/// <returns>描画が必要なDrawResourceのリスト</returns>
	std::vector<DrawResource*> GetDrawResources() const;

	/// <summary>
	/// 区切り文字を設定（例: コロン":"を右から2桁目に挿入）
	/// </summary>
	/// <param name="positions">右から数えた桁数の位置（0始まり）</param>
	/// <param name="separatorTexturePath">区切り文字のテクスチャパス</param>
	void SetSeparator(const std::set<int>& positions, const std::string& separatorTexturePath);

	/// <summary>
	/// 区切り文字をクリア
	/// </summary>
	void ClearSeparators();

	// 設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetPosition(const Vector3& position) { position_ = position; }
	void SetColor(uint32_t color) { color_ = color; }

private:
	// 各桁用のDrawResource
	std::vector<std::unique_ptr<DrawResource>> digitResources_;

	// 区切り文字用のDrawResource
	std::vector<std::unique_ptr<DrawResource>> separatorResources_;
	
	// 区切り文字の位置（右から数えた桁数）
	std::set<int> separatorPositions_;
	
	// 区切り文字のテクスチャパス
	std::string separatorTexturePath_;

	// 表示する数字
	int displayNumber_ = 0;

	// 指定された桁数
	int specifiedDigitCount_ = 0;

	// スケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	// 位置
	Vector3 position_ = { 0.0f, 0.0f, 0.0f };

	// 色
	uint32_t color_ = 0xffffffff;

	// カメラ
	Camera* camera_ = nullptr;

	// 最大桁数
	static constexpr int kMaxDigits = 10;

	bool isTimer_ = false;
};
