#include "BeatManager.h"
#include "externals/imgui/imgui.h"
#include <list>

void BeatManager::Update() {
	UpdateBeatData(datas_[updateIndex_], timers_[updateIndex_]);
}

void BeatManager::ImGuiDraw() {
	ImGui::Begin("Beat Manager");
	ImGui::Text("Update Index: %d", updateIndex_);
	std::vector<const char*> bgms;
	for (auto& data : datas_) {
		if (data) {
			bgms.push_back(data->name.c_str());
		} else {
			bgms.push_back("Empty");
		}
	}
	ImGui::Combo("UpdateBGM", &updateIndex_, bgms.data(), static_cast<int>(bgms.size()));
	ImGui::End();
}

int BeatManager::AddBeatData(std::string name, int soundIndex, bool isBGM, int bpm, int beatCount) {
	int index = -1;
	for (int i = 0; i < datas_.size(); ++i) {
		//空いている場所があったらそこに入れる
		if (!datas_[i]) {
			index = i;
			break;
		}

		//どこも空いてなかったら追加する
		if (i == datas_.size() - 1) {
			index = datas_.size();
			datas_.push_back(nullptr);
			timers_.push_back(-1);
		}
	}

	datas_[index] = std::make_shared<BeatData>();
	datas_[index]->name = name;
	datas_[index]->soundIndex = soundIndex;
	datas_[index]->isBGM = isBGM;
	datas_[index]->bpm = bpm;
	datas_[index]->beatCount = beatCount;
	timers_[index] = 0;

	return index;
}

void BeatManager::DeleteBeatData(int index) {
	if (index < 0 || index >= datas_.size() || !datas_[index]) {
		return; // 無効なインデックス
	}
	datas_[index].reset(); // データを削除
	timers_[index] = -1; // タイマーをリセット
}

void BeatManager::SetUpdateIndex(int index) {
	if (index < 0 || index >= datas_.size() || !datas_[index]) {
		return; // 無効なインデックス
	}
	updateIndex_ = index; // 更新するデータのインデックスを設定
}

void BeatManager::ResetBeatData(int index) {
	if (index < 0 || index >= datas_.size() || !datas_[index]) {
		return; // 無効なインデックス
	}
	timers_[index] = 0; // タイマーをリセット
	datas_[index]->beatCount = 0; // ビートカウントをリセット
}

int BeatManager::GetBeatCount() const {
	if (updateIndex_ < 0 || updateIndex_ >= datas_.size() || !datas_[updateIndex_]) {
		return 0; // 無効なインデックス
	}
	return datas_[updateIndex_]->beatCount;
}

void BeatManager::UpdateBeatData(std::shared_ptr<BeatData> data, int& timer) {
	++timer;
	float second = static_cast<float>(timer) / 60.0f;

	data->beatCount = static_cast<int>(second * data->bpm / 60.0f);
}
