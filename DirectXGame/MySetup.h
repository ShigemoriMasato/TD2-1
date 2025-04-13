#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "Logger.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class MySetup {
public:

	MySetup();

	void Initialize();

private:

	//ウィンドウ作成
	void SamonWindow();

	//DirectX初期化
	void InitializeDirectX();

	std::unique_ptr<Logger> log_;

};

