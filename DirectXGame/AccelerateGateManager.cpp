#include "AccelerateGateManager.h"

AccelerateGateManager::AccelerateGateManager(Camera* camera, CommonData* commonData) {
	camera_ = camera;
	AccelerateGate::SetHandle(commonData->modelHandle_[int(ModelType::AccelerateGate)]);

	debugger_ = std::make_unique<AccelerateGateDebugger>(this);
}

void AccelerateGateManager::Initialize() {

}

void AccelerateGateManager::Update() {
	debugger_->Update();

	for(auto& gate : gates_) {
		gate->Update();
	}
}

void AccelerateGateManager::Draw() {
	for(auto& gate : gates_) {
		gate->Draw();
	}
}

void AccelerateGateManager::MakeGate() {
	auto gate = std::make_shared<AccelerateGate>(camera_);
	gate->Initialize();
	gates_.push_back(gate);
}

void AccelerateGateManager::MakeGateFromConfig(AccelerateGateConfig config) {
	auto gate = std::make_shared<AccelerateGate>(camera_);
	gate->Initialize();
	gate->SetConfig(config);
	gates_.push_back(gate);
}

void AccelerateGateManager::EraseGate(int index) {
	if (index < 0 || index >= gates_.size()) {
		return; // Invalid index
	}
	gates_.erase(gates_.begin() + index);
}

void AccelerateGateManager::ClearGates() {
	gates_.clear();
}
