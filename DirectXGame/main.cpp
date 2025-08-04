#include "Engine/Scene/SceneManager.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	const int32_t kWindowWidth = 1280;
	const int32_t kWindowHeight = 720;

	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>(kWindowWidth, kWindowHeight);
	
	MSG msg{};

	//ウィンドウのxボタンが押されるまでループ
	while (sceneManager->IsRoop()) {
		//更新処理
		sceneManager->Update();

		//描画処理
		sceneManager->Draw();
	}

	return 0;
}
