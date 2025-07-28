#include <Scene/SceneManager.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>(1280, 720);

	while (sceneManager->IsRoop()) {

		sceneManager->Update();

		sceneManager->Draw();

	}

	return 0;
}