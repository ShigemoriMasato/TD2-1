#include "Engine/Scene/SceneManager.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	const int32_t kWindowWidth = 1280;
	const int32_t kWindowHeight = 720;

	SceneManager* sceneManager = new SceneManager(kWindowWidth, kWindowHeight);
	
	MSG msg{};

	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			
			//更新処理

			sceneManager->Update();

			//更新処理

			//描画処理

			sceneManager->Draw();

			//描画処理
		}
	}

	return 0;
}
