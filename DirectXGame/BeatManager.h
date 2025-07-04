#pragma once
#include <vector>
#include <memory>
#include <string>

struct BeatData {
	std::string name;
	int soundIndex;
	bool isBGM;		//true: BGM, false: SE
	int bpm;
	int beatCount;
};

class BeatManager {
public:

	BeatManager() = default;
	~BeatManager() = default;

	void Update();

	void ImGuiDraw();

	int AddBeatData(std::string name, int soundIndex, bool isBGM, int bpm, int beatCount);
	void DeleteBeatData(int index);
	void SetUpdateIndex(int index);
	void ResetBeatData(int index);

	int GetBeatCount() const;

private:

	void UpdateBeatData(std::shared_ptr<BeatData> data, int& timer);

	std::vector<std::shared_ptr<BeatData>> datas_;
	std::vector<int> timers_;
	int updateIndex_ = 0;	//現在更新しているデータのインデックス

};
