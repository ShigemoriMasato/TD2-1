#include "CometManager.h"
#include <imgui.h>

CometManager::CometManager(Camera* camera, Vector3* playerPosPtr, CommonData* commonData) {
	camera_ = camera;
	playerPos_ = playerPosPtr;
	comets_.clear();

	binaryManager_ = std::make_unique<BinaryManager>();

	cometHandles_ = {
		commonData->modelHandle_[int(ModelType::lowCommet)],
		commonData->modelHandle_[int(ModelType::middleCommet)],
		commonData->modelHandle_[int(ModelType::highCommet)],
	};
}

CometManager::~CometManager() {
}

void CometManager::Initialize() {
	LoadCometConfig("Comet.dat");
}

void CometManager::Update() {

	ImGui::Begin("CometManager");

	if (ImGui::Button("Edit")) {

	}

	if(ImGui::Button("Save")) {
		
	}

	if (ImGui::Button("Load")) {
		
	}

	ImGui::End();

	for (auto& comet : comets_) {
		comet->Update();
	}

}

void CometManager::Draw() {
}

void CometManager::SaveCometConfig(std::string filePath) {
	for (auto& comet : comets_) {
		CometConfig Config = comet->GetConfig();
		binaryManager_->RegistOutput(Config, "c");
	}

	binaryManager_->Write(filePath);
}

void CometManager::LoadCometConfig(std::string filePath) {
	//バイナリからデータを読む
	auto cometData = binaryManager_->Read(filePath);

	//読み込んだデータをCometに変換してCometを生成する
	for (auto& data : cometData) {
		CometConfig config = dynamic_cast<Value<CometConfig>&>(*data).value;
		auto comet = std::make_unique<Comet>(camera_, cometHandles_, playerPos_);

		comet->SetConfig(config);
		comets_.push_back(std::move(comet));
	}
}
