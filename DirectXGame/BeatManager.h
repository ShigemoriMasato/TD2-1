#pragma once
#include <vector>
#include <memory>
#include <string>
#include "BPMMeasure.h"
#include "HPBM.h"

struct BeatData {
	std::string name{};
	int soundIndex = -1;
	int bpm = 120;
};

class BeatManager {
public:

	BeatManager();
	~BeatManager();

	void Update();

	void ImGuiDraw();

	void DrawWave(Camera* camera) const;

	int AddBeatData(std::string name, int soundIndex, int bpm);
	void DeleteBeatData(int index);
	void SetUpdateIndex(int index);
	void ResetBeatData(int index);

	void ToggleUpdate() { isUpdate_ = !isUpdate_; }

	int GetBeatCount() const;
	int* GetBeatCountPtr() { return &beatCount_; }

private:

	BPMMeasure* bpmMeasure_ = nullptr;
	HPBM::Measure* hpMeasure_ = nullptr;

	std::vector<std::shared_ptr<BeatData>> data_;
	std::vector<int> timers_;
	int updateIndex_ = 0;	//現在更新しているデータのインデックス
	int beatCount_ = 0;		//現在のビート数

	enum MeasureType {
		None = -1,
		Wave,
		HumanPower
	}measureType_ = HumanPower;

	bool isUpdate_ = false;
};
