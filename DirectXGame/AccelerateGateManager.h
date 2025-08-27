#pragma once
#include "AccelerateGate.h"
#include "Scene/Common/CommonData.h"
#include "AccelerateGateConfig.h"
#include "AccelerateGateDebugger.h"

class AccelerateGateManager {
public:

	AccelerateGateManager(Camera* camera, CommonData* commonData);
	~AccelerateGateManager() {};

	void Initialize();
	void Update();
	void Draw();

	void MakeGate();
	void MakeGateFromConfig(AccelerateGateConfig config);
	void EraseGate(int index);
	void ClearGates();

	std::vector<std::shared_ptr<AccelerateGate>>& GetGates() { return gates_; }

private:

	Camera* camera_;
	std::vector<std::shared_ptr<AccelerateGate>> gates_;

	int gateIndex_ = 0;

	std::unique_ptr<AccelerateGateDebugger> debugger_;

};

