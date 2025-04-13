#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dbghelp.h>
#include "Logger.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception);

class MySetup {
public:

	MySetup(int32_t windowWidth, int32_t windowHeight);

	void Initialize();

private:

	//ウィンドウ作成
	void SamonWindow();

	//DirectX初期化
	void InitializeDirectX();

	const int32_t kClientwidth;
	const int32_t kClientheight;

	HWND hwnd = nullptr;	//ウィンドウハンドル

	std::unique_ptr<Logger> masterLog;
	std::unique_ptr<Logger> dxLog;

};

