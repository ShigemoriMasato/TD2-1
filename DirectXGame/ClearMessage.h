#pragma once
#include "Object/Object.h"
#include "Scene/Common/CommonData.h"
#include "Tools/Binary/BinaryManager.h"

class ClearMessageObj : public Object {
public:

	ClearMessageObj(int handle, Camera* camera);
	~ClearMessageObj();

	void Initialize();
	void Update(Vector3 pos);

private:

	int frame_ = 0;
	bool isRotate_ = true;

};

class ClearMessage {
public:

	ClearMessage(CommonData* cd);
	~ClearMessage();

	void Initialize();
	void Update();
	void Draw();

private:

	std::shared_ptr<Camera> camera_ = nullptr;

	std::vector<std::unique_ptr<ClearMessageObj>> clearMessageObjs_;
	std::vector<Vector3> positions_;

	int frame_ = 0;
	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;

	std::string fileName_ = "ClearMessage.dat";
};
