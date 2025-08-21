#include "CometManager.h"
#include <imgui.h>
#include "CometDebugger.h"

CometManager::CometManager(Camera* camera, Vector3* playerPosPtr, CommonData* commonData) {
	camera_ = camera;
	playerPos_ = playerPosPtr;
	comets_.clear();

	cometHandles_ = {
		commonData->modelHandle_[int(ModelType::lowCommet)],
		commonData->modelHandle_[int(ModelType::middleCommet)],
		commonData->modelHandle_[int(ModelType::highCommet)],
	};

	debugger_ = std::make_unique<CometDebugger>(this);
}

CometManager::~CometManager() {
}

void CometManager::Initialize() {
}

void CometManager::Update() {

	debugger_->Update();

	for (auto& comet : comets_) {
		comet->Update();
	}

}

void CometManager::Draw() {
	for(auto& comet : comets_) {
		comet->Draw();
	}
}

void CometManager::MakeComet() {
	auto comet = std::make_shared<Comet>(camera_, cometHandles_, playerPos_);
	comets_.push_back(std::move(comet));
}

void CometManager::MakeCometfromConfig(const CometConfig& config) {
	auto comet = std::make_shared<Comet>(camera_, cometHandles_, playerPos_);
	comet->SetConfig(config);
	comets_.push_back(std::move(comet));
}

void CometManager::ClearComet() {
	comets_.clear();
}
