#pragma once
#include "Comet.h"
#include "Tools/Binary/BinaryManager.h"
#include <memory>

class CometManager;

class CometDebugger {
public:
	CometDebugger(CometManager* cometManager);
	~CometDebugger();

	void Update();

private:
	void SaveCometConfig(std::string filePath);
	void LoadCometConfig(std::string filePath);

	std::unique_ptr<BinaryManager> binaryManager_;
	std::vector<std::string> configFileNames_;

	CometManager* manager_ = nullptr;

	char newFileName_[128];

	int selectedCometIndex_ = -1;
	int selectedCometConfigIndex_ = -1;
};

