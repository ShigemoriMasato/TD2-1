#include <Core/EngineTerminal.h>

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	std::unique_ptr<EngineTerminal> engine = std::make_unique<EngineTerminal>(BootMode::Game);
	engine->Initialize(1280, 720);

	engine->Run();

	return 0;
}
