#include "InitializeScene.h"
#include <Game/Scene/GameScene.h>
#include <Game/Scene/TitleScene.h>
#include <Math/MyString.h>

void InitializeScene::Initialize() {
	LoadAllResources();

	auto keyManager = commonData->keyManager_.get();

	keyManager->SetKey(Key::Right, DIK_D, KeyState::Hold);
	keyManager->SetKey(Key::Right, DIK_RIGHTARROW, KeyState::Hold);
	keyManager->SetButton(Key::Right, XBoxController::kRight, KeyState::Hold);
	keyManager->SetStick(Key::Right, true, false, 0.5f);

	keyManager->SetKey(Key::Left, DIK_A, KeyState::Hold);
	keyManager->SetKey(Key::Left, DIK_LEFTARROW, KeyState::Hold);
	keyManager->SetButton(Key::Left, XBoxController::kLeft, KeyState::Hold);
	keyManager->SetStick(Key::Left, true, false, -0.5f);

	keyManager->SetKey(Key::Up, DIK_W, KeyState::Hold);
	keyManager->SetKey(Key::Up, DIK_UPARROW, KeyState::Hold);
	keyManager->SetButton(Key::Up, XBoxController::kUp, KeyState::Hold);
	keyManager->SetStick(Key::Up, true, true, 0.5f);

	keyManager->SetKey(Key::Down, DIK_S, KeyState::Hold);
	keyManager->SetKey(Key::Down, DIK_DOWNARROW, KeyState::Hold);
	keyManager->SetButton(Key::Down, XBoxController::kDown, KeyState::Hold);
	keyManager->SetStick(Key::Down, true, true, -0.5f);

	keyManager->SetKey(Key::Action, DIK_SPACE, KeyState::Hold);
	keyManager->SetKey(Key::Action, DIK_J, KeyState::Hold);
	keyManager->SetKey(Key::Action, DIK_Z, KeyState::Hold);
	keyManager->SetKey(Key::Action, DIK_X, KeyState::Hold);
	keyManager->SetKey(Key::Action, DIK_LSHIFT, KeyState::Hold);
	keyManager->SetKey(Key::Action, DIK_RSHIFT, KeyState::Hold);
	keyManager->SetKey(Key::Action, DIK_LCONTROL, KeyState::Hold);
	keyManager->SetKey(Key::Action, DIK_RCONTROL, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kA, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kB, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kY, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kX, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kRightShoulder, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kRightTrigger, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kLeftShoulder, KeyState::Hold);
	keyManager->SetButton(Key::Action, XBoxController::kLeftTrigger, KeyState::Hold);

	keyManager->SetKey(Key::Correct, DIK_RETURN, KeyState::Trigger);
	keyManager->SetKey(Key::Correct, DIK_SPACE, KeyState::Trigger);
	keyManager->SetKey(Key::Correct, DIK_Z, KeyState::Trigger);
	keyManager->SetButton(Key::Correct, XBoxController::kA, KeyState::Trigger);

	keyManager->SetKey(Key::Reverse, DIK_ESCAPE, KeyState::Trigger);
	keyManager->SetKey(Key::Reverse, DIK_X, KeyState::Trigger);
	keyManager->SetButton(Key::Reverse, XBoxController::kB, KeyState::Trigger);

	keyManager->SetKey(Key::Debug, DIK_F1, KeyState::Trigger);
	keyManager->SetKey(Key::Debug, DIK_ESCAPE, KeyState::Trigger);

	keyManager->SetKey(Key::DebugClear, DIK_F2, KeyState::Trigger);


	int game = audio_->Load("BGM/Game.mp3");
	audio_->SetVolume(game, 1.0f);
	int slow = audio_->Load("slow.mp3");
	audio_->SetVolume(slow, 1.0f);
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

	commonData->isCreateTexture = false;
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
