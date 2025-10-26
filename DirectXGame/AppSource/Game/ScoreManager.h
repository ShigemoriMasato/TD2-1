#pragma once
#include <Tools/Binary/BinaryManager.h>

class ScoreManager {
public:

	ScoreManager();
	~ScoreManager();

	/// <summary>
	/// スコアの保存。flushはデストラクタ内で行う
	/// </summary>
	/// <param name="stage">保存するステージ</param>
	/// <param name="score">保存するスコア</param>
	void SaveScore(int stage, int score);

	/// <summary>
	/// 保存してあるすべてのスコアを取得
	/// </summary>
	std::vector<int> GetAllScores();

	/// <summary>
	/// Scoreの取得
	/// </summary>
	/// <param name="stage">取得したいステージ</param>
	/// <returns>スコア</returns>
	int GetScore(int stage);

private:

	std::unique_ptr<BinaryManager> binaryManager_;
	std::vector<int> scores_;
	const std::string saveFile = "data.sav";
};

