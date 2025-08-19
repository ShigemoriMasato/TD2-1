#pragma once
#include "Comet.h"
#include "../Scene/Common/CommonData.h"
#include "../Tools/Binary/BinaryManager.h"
#include "CometConfig.h"
#include <memory>

class CometManager {
public:

	CometManager(Camera* camera, Vector3* playerPosPtr, CommonData* commonData);
	~CometManager();

	void Initialize();

	void Update();

	void Draw();

private:

	void SaveCometConfig(std::string filePath);
	void LoadCometConfig(std::string filePath);

	std::unique_ptr<BinaryManager> binaryManager_;

	Camera* camera_;
	Vector3* playerPos_;
	std::vector<std::unique_ptr<Comet>> comets_;

	std::vector<std::string> cometConfigFileNames_;

	std::array<int, 3> cometHandles_ = { -1, -1, -1 };
};

