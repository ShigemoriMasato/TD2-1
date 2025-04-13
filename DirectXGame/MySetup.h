#pragma once
#include <Windows.h>
#include "Logger.h"
#include "MyDirectX.h"

class MySetup {
public:

	MySetup(int32_t windowWidth, int32_t windowHeight);

	void Initialize();

private:

	//ウィンドウ作成
	void SamonWindow();

	std::unique_ptr<MyDirectX> dx = nullptr;

	const int32_t kClientwidth;
	const int32_t kClientheight;

	HWND hwnd = nullptr;	//ウィンドウハンドル

	std::unique_ptr<Logger> masterLog;

};

