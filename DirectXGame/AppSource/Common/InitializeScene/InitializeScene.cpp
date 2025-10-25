#include "InitializeScene.h"
#include <Game/Scene/GameScene.h>
#include <Game/Scene/TitleScene.h>
#include <Math/MyString.h>

void InitializeScene::Initialize() {
	//LoadAllResources();
}

std::unique_ptr<BaseScene> InitializeScene::Update() {
	////ぶっちゃけもうImGui使わないけど一応デバッグとして残す
	//ImGui::Begin("StageSelect");
	//auto gpuHandle = offScreenManager_->GetOffScreenData(OffScreenIndex::Level1)->GetTextureGPUHandle();
	//ImGui::Image(ImTextureID(gpuHandle.ptr), ImVec2(720, 415));
	//ImGui::End();
	return std::make_unique<TitleScene>();
}

void InitializeScene::Draw() {
	if (isfirst_) {
		CreateStageTexture();
		isfirst_ = false;
	}
}

void InitializeScene::CreateStageTexture() {
	Transform transform;
	auto levelFileMap = LevelLoader::GetLevelFileMap();
	commonData->isCreateTexture = true;

	for (const auto level : levelFileMap) {
		commonData->nextLevelIndex_ = level.first;
		std::unique_ptr<SceneManager> sceneManager_ = std::make_unique<SceneManager>(std::make_unique<GameScene>(), this);
		sceneManager_->SetCommonData(commonData);
		sceneManager_->Update();
		sceneManager_->Draw();
		sceneList_.push_back(std::move(sceneManager_));
	}
}

void InitializeScene::LoadAllResources() {

	auto textureFile = SerchFilePathsAddChild("Assets/Texture", ".png");

	for (const auto& filePath : textureFile) {
		std::string relativePath = "Assets/Texture/" + filePath;
		textureManager_->LoadTexture(relativePath);
	}

	auto audioFiles = SerchFilePathsAddChild("Assets/Audio");

	for (const auto& filePath : audioFiles) {
		std::string relativePath = filePath;
		audio_->Load(relativePath);
	}

	auto modelFilesObj = SearchFileNames("Assets/Model");

	for (const auto& filePath : modelFilesObj) {
		std::string relativePath = filePath;
		modelManager_->LoadModel(relativePath);
	}

	auto modelFilesGltf = SearchFileNames("Assets/Model");

	for (const auto& filePath : modelFilesGltf) {
		std::string relativePath = filePath;
		modelManager_->LoadModel(relativePath);
	}

	auto modelFilesGlb = SearchFileNames("Assets/Model");

	for (const auto& filePath : modelFilesGlb) {
		std::string relativePath = filePath;
		modelManager_->LoadModel(relativePath);
	}

}
