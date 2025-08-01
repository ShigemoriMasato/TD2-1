#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"
#include "../BluePrint/Struct.h"
#include "../BluePrint/Function.h"

class GameScene : public Scene {
public:
	GameScene(std::shared_ptr<CommonData> commonData);
	~GameScene();

	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:

	Camera* camera_;
	DebugCamera* debugCamera_;

	bool isDebugCamera = false;

	std::vector<ValueBase*> values_{};

	int typeIndex{};

	int intbuffer{};
	float floatbuffer{};
	Vector2 vector2buffer{};
	Vector3 vector3buffer{};
	char valueNamebuffer[256]{};
	char structNamebuffer[256]{};

	std::vector<ValueBase*> args_;
	char functionNamebuffer_[256]{};
	std::vector<Function> funcs_;

	std::vector<Value<Struct>> structs_;
};
