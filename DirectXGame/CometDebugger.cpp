#include "CometDebugger.h"
#include "CometManager.h"
#include <algorithm>

CometDebugger::CometDebugger(CometManager* cometManager) {
	cometManager_ = cometManager;
	binaryManager_ = std::make_unique<BinaryManager>();

	auto files = binaryManager_->Read("Comet/CometFile.dat");

	for (auto& file : files) {
		cometConfigFileNames_.push_back(dynamic_cast<Value<std::string>&>(*file).value);
	}
}

CometDebugger::~CometDebugger() {
	for (auto& file : cometConfigFileNames_) {
		binaryManager_->RegistOutput(file, "s");
	}
	binaryManager_->Write("Comet/CometFile.dat");
}

void CometDebugger::Update() {
	
	ImGui::Begin("Comet Debugger");

#pragma region Comet操作

	auto comets = cometManager_->GetComets();
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
		Vector3* posPtr = cometManager_->GetComets()[selectedCometIndex_]->GetPositionPtr();
		ImGui::DragFloat3("Position", &posPtr->x);

		ImGui::End();
	}

#pragma endregion

#pragma region Comet生成

	if (ImGui::Button("Make Comet")) {
		cometManager_->MakeComet();
	}
	if (ImGui::Button("Clear Comets")) {
		cometManager_->ClearComet();
		selectedCometIndex_ = 0;
	}

#pragma endregion

#pragma region 外部ファイル関連

	std::vector<const char*> fileNames;

	for(const auto& file : cometConfigFileNames_) {
		fileNames.push_back(file.c_str());
	}

	ImGui::Combo("ConfigFile", &selectedCometConfigIndex_, fileNames.data(), int(fileNames.size()));

	if (ImGui::Button("Load")) {
		LoadCometConfig(cometConfigFileNames_[selectedCometConfigIndex_]);
	}

	if(ImGui::Button("Save")) {
		SaveCometConfig(cometConfigFileNames_[selectedCometConfigIndex_]);
	}

	if (ImGui::Button("Delete")) {
		cometConfigFileNames_.erase(cometConfigFileNames_.begin() + selectedCometConfigIndex_);
		selectedCometConfigIndex_ = 0;
	}

#pragma endregion

#pragma region 新規ファイル作成

	ImGui::InputText("NewFile", newFileName_, 128);

	if (ImGui::Button("MakeNewFile")) {
		cometConfigFileNames_.push_back(newFileName_);
		binaryManager_->MakeFile("Comet/" + std::string(newFileName_));

		for (auto& file : cometConfigFileNames_) {
			binaryManager_->RegistOutput(file, "s");
		}
		binaryManager_->Write("Comet/CometFile.dat");
	}

#pragma endregion

	ImGui::End();

}

void CometDebugger::SaveCometConfig(std::string filePath) {
	for (auto& comet : cometManager_->GetComets()) {
		CometConfig config = comet->GetConfig();
		binaryManager_->RegistOutput(config, "c");
	}

	binaryManager_->Write("Comet/" + filePath);
}

void CometDebugger::LoadCometConfig(std::string filePath) {
	//Cometを全て削除
	cometManager_->ClearComet();

	//バイナリからデータを読む
	auto cometData = binaryManager_->Read("Comet/" + filePath);

	//読み込んだデータをCometに変換してCometを生成する
	for (auto& data : cometData) {
		CometConfig config = dynamic_cast<Value<CometConfig>&>(*data).value;
		cometManager_->MakeCometfromConfig(config);
	}
}
