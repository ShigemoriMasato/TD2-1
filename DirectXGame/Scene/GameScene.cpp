#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
}

GameScene::~GameScene() {
}

void GameScene::Initialize() {
}

Scene* GameScene::Update() {

	return nullptr;
}

void GameScene::Draw() const {
	
}
