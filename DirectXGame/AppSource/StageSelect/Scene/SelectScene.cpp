#include "SelectScene.h"
#include <Game/Tile/TileMap.h>
#include <Game/Physics/PhysicsEngine.h>

SelectScene::SelectScene() {
}

SelectScene::~SelectScene() {
}

void SelectScene::Initialize() {
	stageMap_[0] = "level1";

	if (firstLoad_) {
		firstLoad_ = false;


	}
}

std::unique_ptr<BaseScene> SelectScene::Update() {

}

void SelectScene::Draw() {
}

void SelectScene::CreateStageTexture() {
	PhysicsEngine physicsEngine;
	TileMap map(&physicsEngine);
	
	for(int i = 0; i < stageMap_.size(); ++i) {
		map.LoadMap("Assets/Map/" + stageMap_[i]);
	}
}
