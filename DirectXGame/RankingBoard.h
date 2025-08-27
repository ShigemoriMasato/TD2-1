#pragma once
#include "Tools/Binary/BinaryManager.h"

class RankingBoard {
public:

	RankingBoard() = default;
	~RankingBoard() = default;

	void Regist(long long time);
	std::list<double> GetRankingList();

private:

	std::unique_ptr<BinaryManager> binaryManager{ std::make_unique<BinaryManager>() };
	std::string fileName_ = "Ranking.dat";

	const int maxRanking_ = 9;
};

