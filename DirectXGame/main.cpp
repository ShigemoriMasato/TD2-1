#include "Engine/Scene/SceneManager.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>(1280, 720);
	
	//ウィンドウのxボタンが押されるまでループ
	while (sceneManager->IsRoop()) {
		//更新処理
		sceneManager->Update();

		//描画処理
		sceneManager->Draw();
	}

	return 0;
}
