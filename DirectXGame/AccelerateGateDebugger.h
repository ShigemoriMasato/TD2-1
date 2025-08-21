#pragma once
#include "AccelerateGate.h"
#include "Tools/Binary/BinaryManager.h"

class AccelerateGateManager;

class AccelerateGateDebugger {
public:
	AccelerateGateDebugger(AccelerateGateManager* manager);
	~AccelerateGateDebugger();

	void Update();

private:

	void SaveState(std::string filename);
	void LoadState(std::string filename);

	std::unique_ptr<BinaryManager> binaryManager_;
	std::vector<std::string> configFileNames_;

	AccelerateGateManager* manager_;

	char newFileName_[128]{};

	int selectedGateIndex_ = 0;
	int selectedConfigIndex_ = 0;

	std::string basePath_ = "AccelerateGate/";
};

