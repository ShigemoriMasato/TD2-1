#include <Windows.h>
#include <cstdint>
#include <iostream>

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	const int32_t kWindowWidth = 1280;
	const int32_t kWindowHeight = 720;

	std::cout << "Window Width: " << kWindowWidth << ", Height: " << kWindowHeight << std::endl;

	return 0;
}
