#include "AccelerateGateDebugger.h"
#include "AccelerateGateManager.h"
#include <algorithm>

AccelerateGateDebugger::AccelerateGateDebugger(AccelerateGateManager* manager) {
	manager_ = manager;

	binaryManager_ = std::make_unique<BinaryManager>();
	
	auto files = binaryManager_->Read(basePath_ + "ConfigFiles.dat");
	for (auto& file : files) {
		configFileNames_.push_back(dynamic_cast<Value<std::string>&>(*file).value);
	}

	LoadState("AccelerationGate.dat", true);
}

AccelerateGateDebugger::~AccelerateGateDebugger() {
}

void AccelerateGateDebugger::Update() {

	ImGui::Begin("Accelerate Gate Debugger");

#pragma region Gate操作

	auto gates = manager_->GetGates();

	std::vector<std::string> idStrings;
	std::vector<const char*> ids;

	for (int i = 0; i < gates.size(); ++i) {
		idStrings.push_back(std::to_string(i));
	}

	for (auto& id : idStrings) {
		ids.push_back(id.c_str());
	}

	ImGui::Combo("Gate ID", &selectedGateIndex_, ids.data(), static_cast<int>(ids.size()));
	selectedGateIndex_ = std::clamp(selectedGateIndex_, 0, std::max(0, static_cast<int>(gates.size() - 1)));

	if(gates.size() > 0) {
		auto& gate = gates[selectedGateIndex_];
		
		auto gateConfig = gate->GetConfig();

		ImGui::Begin("GateEdit");

		ImGui::Text("Gate ID: %d", selectedGateIndex_);

		ImGui::DragFloat3("scale", &gateConfig.transform.scale.x, 0.01f);
		ImGui::DragFloat3("rotation", &gateConfig.transform.rotation.x, 0.01f);
		ImGui::DragFloat3("position", &gateConfig.transform.position.x, 0.1f);

		if (ImGui::Button("Erase")) {
			manager_->EraseGate(selectedGateIndex_);
			selectedGateIndex_ = std::max(0, selectedGateIndex_ - 1);
		}

		if (ImGui::Button("SetCameraPos")) {
			 gateConfig.transform.position = gate->GetCameraPos();
		}

		ImGui::End();

		gate->SetConfig(gateConfig);
	}

#pragma endregion

#pragma region 生成

	if (ImGui::Button("Generate")) {
		manager_->MakeGate();
	}
	if(ImGui::Button("Clear")) {
		manager_->ClearGates();
	}

#pragma endregion

#pragma region 外部ファイル

	std::vector<const char*> fileNames;

	for (const auto& file : configFileNames_) {
		fileNames.push_back(file.c_str());
	}

	ImGui::Combo("ConfigFile", &selectedConfigIndex_, fileNames.data(), int(fileNames.size()));

	if (ImGui::Button("Load")) {
		LoadState(configFileNames_[selectedConfigIndex_], true);
	}

	if (ImGui::Button("Making")) {
		LoadState(configFileNames_[selectedConfigIndex_], false);
	}

	if (ImGui::Button("Save")) {
		SaveState(configFileNames_[selectedConfigIndex_]);
	}

	if (ImGui::Button("Delete")) {
		configFileNames_.erase(configFileNames_.begin() + selectedConfigIndex_);
		selectedConfigIndex_ = 0;

		for (auto& file : configFileNames_) {
			binaryManager_->RegistOutput(file, "s");
		}
		binaryManager_->Write(basePath_ + "ConfigFiles.dat");
	}

#pragma endregion

#pragma region 新規ファイル作成

	ImGui::InputText("NewFile", newFileName_, 128);

	if (ImGui::Button("MakeNewFile")) {
		configFileNames_.push_back(newFileName_);
		binaryManager_->MakeFile(basePath_ + std::string(newFileName_));

		for (auto& file : configFileNames_) {
			binaryManager_->RegistOutput(file, "s");
		}
		binaryManager_->Write(basePath_ + "ConfigFiles.dat");
	}

#pragma endregion

	ImGui::End();

}

void AccelerateGateDebugger::SaveState(std::string filename) {
	auto gates = manager_->GetGates();

	for (auto& g : gates) {
		binaryManager_->RegistOutput(g->GetConfig(), "g");
	}

	binaryManager_->Write(basePath_ + filename);
}

void AccelerateGateDebugger::LoadState(std::string filename, bool isClear) {
	if (isClear) {
		manager_->ClearGates();
	}

	auto configs = binaryManager_->Read(basePath_ + filename);

	for (auto& config : configs) {
		auto gateConfig = dynamic_cast<Value<AccelerateGateConfig>&>(*config).value;
		manager_->MakeGateFromConfig(gateConfig);
	}
}
