#pragma once
#include "Comet.h"
#include "../Scene/Common/CommonData.h"
#include "../Tools/Binary/BinaryManager.h"
#include "CometConfig.h"
#include <memory>

class CometDebugger;

class CometManager {
public:

	CometManager(Camera* camera, Vector3* playerPosPtr, CommonData* commonData);
	~CometManager();

	void Initialize();
	void Update();
	void Draw();

	void MakeComet();
	void MakeCometfromConfig(const CometConfig& config);
	void ClearComet();

	std::vector<std::shared_ptr<Comet>>& GetComets() { return comets_; }

private:

	Camera* camera_;
	Vector3* playerPos_;
	std::vector<std::shared_ptr<Comet>> comets_;

	std::array<int, 3> cometHandles_ = { -1, -1, -1 };

	std::unique_ptr<CometDebugger> debugger_;
};

