#include "MySetup.h"
#include "Logger.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<MySetup> setup = std::make_unique<MySetup>(1280, 720);

	setup->Initialize();

	MSG msg{};

	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//ゲームの更新処理
		}
	}

	return 0;
}
