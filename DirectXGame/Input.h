#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "MyMath.h"

class Input {
public:
	Input() = default;
	~Input() = default;

	void SetHInstance(HINSTANCE hInstance) {
		hInstance_ = hInstance;
	}

	void SetHWND(HWND hwnd) {
		hwnd_ = hwnd;
	}

	void Initialize();

	void Update();

	static BYTE* GetKeyState() {
		return keyState;
	}

	static BYTE* GetPreKeyState() {
		return preKeyState;
	}

	static BYTE GetKeyState(int keyCode);

	static BYTE GetPreKeyState(int keyCode);

	static Vector2 GetMouseMove();

	static BYTE* GetMouseButtonState();

	static BYTE* GetPreMouseButtonState();

private:

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_ = nullptr;

	static BYTE keyState[256];
	static BYTE preKeyState[256];

	static DIMOUSESTATE preMouseState;
	static DIMOUSESTATE mouseState;

	HINSTANCE hInstance_ = nullptr;
	HWND hwnd_ = nullptr;
};
