#pragma once
#include <vector>
#include <memory>
#include <string>
#include "BPMMeasure.h"

struct BeatData {
	std::string name{};
	int soundIndex = -1;
	int bpm = 120;
};

class BeatManager {
public:

	BeatManager(BPMMeasure* pMasure = nullptr) : bpmMeasure_(pMasure) {};
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

private:

	BPMMeasure* bpmMeasure_ = nullptr;

	std::vector<std::shared_ptr<BeatData>> data_;
	std::vector<int> timers_;
	int updateIndex_ = 0;	//現在更新しているデータのインデックス
	int beatCount_ = 0;		//現在のビート数

	bool isUpdate_ = false;
};
