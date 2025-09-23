#include <Scene/SceneManager.h>

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();

	while(sceneManager->IsLoop()) {

		sceneManager->Update();

		sceneManager->Draw();
	}

	return 0;
}
