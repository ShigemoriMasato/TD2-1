#include "MyDirectX.h"
#include "MyMath.h"
#include "MyEngine.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	const float kWindowWidth = 1280.0f;
	const float kWindowHeight = 720.0f;

	MyDirectX* myDirectX = new MyDirectX(int(kWindowWidth), int(kWindowHeight));

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

			//更新処理

			//描画処理

			//描画処理

			myDirectX->EndFrame();
		}
	}

	delete myDirectX;

	return 0;
}
