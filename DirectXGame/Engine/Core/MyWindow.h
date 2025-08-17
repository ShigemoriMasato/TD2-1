#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <functional>

class MyWindow {
public:

	MyWindow(const int32_t kClientWidth, const int32_t kClientHeight);
	~MyWindow();
	HWND GetHwnd(int handle);
	WNDCLASS GetWndClass(int handle);

	[[nodiscard]]
	int CreateWindowForApp();

	static LRESULT CALLBACK WindowProcCarrier(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

	//hwndを統括するためにstaticを使用
	static std::vector<HWND> hwndList_;
	static std::vector<WNDCLASS> wcList_;

	//ウィンドウプロシージャをstaticにすることで、インスタンス化せずにアクセスできるようにする
	static std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> windowProc_;
	const int32_t kClientWidth_;
	const int32_t kClientHeight_;

};
