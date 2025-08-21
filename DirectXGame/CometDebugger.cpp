#include "CometDebugger.h"
#include "CometManager.h"
#include <algorithm>

CometDebugger::CometDebugger(CometManager* cometManager) {
	manager_ = cometManager;
	binaryManager_ = std::make_unique<BinaryManager>();

	auto files = binaryManager_->Read("Comet/ConfigFile.dat");

	for (auto& file : files) {
		configFileNames_.push_back(dynamic_cast<Value<std::string>&>(*file).value);
	}
}

CometDebugger::~CometDebugger() {
	for (auto& file : configFileNames_) {
		binaryManager_->RegistOutput(file, "s");
	}
	binaryManager_->Write("Comet/CometFile.dat");
}

void CometDebugger::Update() {
	
	ImGui::Begin("Comet Debugger");

#pragma region Comet操作

	auto comets = manager_->GetComets();
	std::vector<std::string> cometIDStrings;
	std::vector<const char*> cometIDs;

	for (int i = 0; i < comets.size(); ++i) {
		cometIDStrings.push_back(std::to_string(i));
	}

	for (auto& id : cometIDStrings) {
		cometIDs.push_back(id.c_str());
	}

	//操作するCometを選択
	ImGui::Combo("target", &selectedCometIndex_, cometIDs.data(), int(cometIDs.size()));
	selectedCometIndex_ = std::clamp(selectedCometIndex_, 0, int(comets.size()));

	//nullじゃなかったら編集画面を開く
	if (comets.size() > 0) {
		ImGui::Begin("CometEdit");

		ImGui::Text("Comet ID: %d", selectedCometIndex_);

		//position
		Vector3* posPtr = manager_->GetComets()[selectedCometIndex_]->GetPositionPtr();
		ImGui::DragFloat3("Position", &posPtr->x, 0.1f);

		ImGui::End();
	}

#pragma endregion

#pragma region Comet生成

	if (ImGui::Button("Make Comet")) {
		manager_->MakeComet();
	}
	if (ImGui::Button("Clear Comets")) {
		manager_->ClearComet();
		selectedCometIndex_ = 0;
	}

#pragma endregion

#pragma region 外部ファイル関連

	std::vector<const char*> fileNames;

	for(const auto& file : configFileNames_) {
		fileNames.push_back(file.c_str());
	}

	ImGui::Combo("ConfigFile", &selectedCometConfigIndex_, fileNames.data(), int(fileNames.size()));

	if (ImGui::Button("Load")) {
		LoadCometConfig(configFileNames_[selectedCometConfigIndex_]);
	}

	if(ImGui::Button("Save")) {
		SaveCometConfig(configFileNames_[selectedCometConfigIndex_]);
	}

	if (ImGui::Button("Delete")) {
		configFileNames_.erase(configFileNames_.begin() + selectedCometConfigIndex_);
		selectedCometConfigIndex_ = 0;
	}

#pragma endregion

#pragma region 新規ファイル作成

	ImGui::InputText("NewFile", newFileName_, 128);

	if (ImGui::Button("MakeNewFile")) {
		configFileNames_.push_back(newFileName_);
		binaryManager_->MakeFile("Comet/" + std::string(newFileName_));

		for (auto& file : configFileNames_) {
			binaryManager_->RegistOutput(file, "s");
		}
		binaryManager_->Write("Comet/CometFile.dat");
	}

#pragma endregion

	ImGui::End();

}

void CometDebugger::SaveCometConfig(std::string filePath) {
	for (auto& comet : manager_->GetComets()) {
		CometConfig config = comet->GetConfig();
		binaryManager_->RegistOutput(config, "c");
	}

	binaryManager_->Write("Comet/" + filePath);
}

void CometDebugger::LoadCometConfig(std::string filePath) {
	//Cometを全て削除
	manager_->ClearComet();

	//バイナリからデータを読む
	auto cometData = binaryManager_->Read("Comet/" + filePath);

	//読み込んだデータをCometに変換してCometを生成する
	for (auto& data : cometData) {
		CometConfig config = dynamic_cast<Value<CometConfig>&>(*data).value;
		manager_->MakeCometfromConfig(config);
	}
}
