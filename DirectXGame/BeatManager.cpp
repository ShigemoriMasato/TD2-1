#include "BeatManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/Sound/Sound.h"
#include <list>

BeatManager::~BeatManager() {
	if (bpmMeasure_) {
		delete bpmMeasure_;
	}
}

void BeatManager::Update() {
	if (isUpdate_) {
		++timers_[updateIndex_];
		float second = static_cast<float>(timers_[updateIndex_]) / 60.0f;

		beatCount_ = static_cast<int>(second * data_[updateIndex_]->bpm / 60.0f);
	}

	if (bpmMeasure_) {
		bpmMeasure_->Measure();

		float bpm = bpmMeasure_->GetBPM();
		if (bpm > 0.0f) {
			data_[updateIndex_]->bpm = static_cast<int>(bpm);
		}
	}
}

void BeatManager::ImGuiDraw() {
	// Update BGM
	ImGui::Begin("Beat Manager");
	ImGui::Text("Update : %s", data_[updateIndex_]->name.c_str());
	std::vector<const char*> bgms;
	for (auto& data : data_) {
		if (data) {
			bgms.push_back(data->name.c_str());
		} else {
			bgms.push_back("Empty");
		}
	}
	ImGui::Combo("UpdateBGM", &updateIndex_, bgms.data(), static_cast<int>(bgms.size()));
	ImGui::Text("BPM : %d", data_[updateIndex_]->bpm);
	ImGui::Text("Sound Index : %d", data_[updateIndex_]->soundIndex);

	// button
	std::string buttonName{};
	if (isUpdate_) {
		buttonName = "Stop";
	} else {
		buttonName = "Start";
	}
	if (ImGui::Button(buttonName.c_str())) {
		isUpdate_ = !isUpdate_;
	};
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		ResetBeatData(updateIndex_);
	};
	ImGui::SameLine();
	if (ImGui::Button("Measure")) {
		if (bpmMeasure_) {
			bpmMeasure_->MeasureStart(updateIndex_);
		}
	}

	// Info
	ImGui::Text("Beat Count : %d", beatCount_);
	ImGui::Text("Timer : %d", timers_[updateIndex_]);

	if (ImGui::Button("play")) {
		Sound::isPlay_[updateIndex_] = true;
	}

	ImGui::End();
}

void BeatManager::DrawWave(Camera* camera) const {
	bpmMeasure_->DrawWave(camera);
}

int BeatManager::AddBeatData(std::string name, int soundIndex, int bpm) {
	int index = -1;
	for (int i = 0; i < data_.size(); ++i) {
		//空いている場所があったらそこに入れる
		if (!data_[i]) {
			index = i;
			break;
		}

		//どこも空いてなかったら追加する
		if (i == data_.size() - 1) {
			index = static_cast<int>(data_.size());
			data_.push_back(nullptr);
			timers_.push_back(-1);
		}
	}

	if (data_.size() == 0) {
		index = static_cast<int>(data_.size());
		data_.push_back(nullptr);
		timers_.push_back(-1);
	}

	data_[index] = std::make_shared<BeatData>();
	data_[index]->name = name;
	data_[index]->soundIndex = soundIndex;
	data_[index]->bpm = bpm;
	timers_[index] = 0;

	return index;
}

void BeatManager::DeleteBeatData(int index) {
	if (index < 0 || index >= data_.size() || !data_[index]) {
		return; // 無効なインデックス
	}
	data_[index].reset(); // データを削除
	timers_[index] = -1; // タイマーをリセット
}

void BeatManager::SetUpdateIndex(int index) {
	if (index < 0 || index >= data_.size() || !data_[index]) {
		return; // 無効なインデックス
	}
	updateIndex_ = index; // 更新するデータのインデックスを設定
}

void BeatManager::ResetBeatData(int index) {
	if (index < 0 || index >= data_.size() || !data_[index]) {
		return; // 無効なインデックス
	}
	timers_[index] = 0; // タイマーをリセット
	beatCount_ = 0; // ビートカウントをリセット
}

int BeatManager::GetBeatCount() const {
	return beatCount_;
}
