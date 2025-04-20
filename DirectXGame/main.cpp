#include "MyDirectX.h"
#include "MyMath.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	const float kWindowWidth = 1280.0f;
	const float kWindowHeight = 720.0f;

	MyDirectX* myDirectX = new MyDirectX(int(kWindowWidth), int(kWindowHeight));
	myDirectX->Initialize();

	Transform transform = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Transform camera = {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -5.0f };
	Matrix4x4 wvpMatrix;

	MSG msg{};

	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {

			transform.rotation.y += 0.03f;
			Matrix4x4 worldMatrix = MakeTransformMatrix(transform);
			Matrix4x4 cameraMatrix = MakeTransformMatrix(camera);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
			wvpMatrix = worldMatrix * viewMatrix * projectionMatrix;

			//ゲームの更新処理
			myDirectX->ClearScreen();
			myDirectX->DrawTriangle(wvpMatrix);
			myDirectX->EndFrame();
		}
	}

	myDirectX->Finalize();
	delete myDirectX;

	return 0;
}
