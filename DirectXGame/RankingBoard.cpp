#include "RankingBoard.h"
#include <Data/UniqueNumber.h>

void RankingBoard::Regist(long long time) {
	double adjusttime = static_cast<double>(time / 1000.0);

	std::list<double> ranking = GetRankingList();

	ranking.push_back(adjusttime);

	for (int i = static_cast<int>(ranking.size()); i < maxRanking_ - 1; ++i) {
		ranking.push_back(UniqueNumber::Double::max);
	}

	ranking.sort();
	ranking.resize(maxRanking_);

	for (auto& data : ranking) {
		binaryManager->RegistOutput(data, "t");
	}

	binaryManager->Write(fileName_);
}

std::list<double> RankingBoard::GetRankingList() {
	auto data = binaryManager->Read(fileName_);

	std::list<double> ranking;
	for (auto d : data) {
		ranking.push_back(dynamic_cast<Value<double>*>(d.get())->value);
	}

	return ranking;
}
