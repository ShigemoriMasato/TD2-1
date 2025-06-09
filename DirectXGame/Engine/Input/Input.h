#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>

#include "../Math/MyMath.h"

class Input {
public:
	Input(HINSTANCE hInstance, HWND hwnd) : hInstance_(hInstance), hwnd_(hwnd) {}
	~Input() = default;

	void SetHWND(HWND hwnd) {
		hwnd_ = hwnd;
	}

	void Initialize();

	void Update();

	static BYTE* GetKeyState();

	static BYTE* GetPreKeyState();

	static BYTE GetKeyState(int keyCode);

	static BYTE GetPreKeyState(int keyCode);

	static Vector2 GetMouseMove();

	static float GetMouseWheel();

	static BYTE* GetMouseButtonState();

	static BYTE* GetPreMouseButtonState();

private:

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_ = nullptr;

	//どこからでもGetできるようにstaticにする
	static BYTE keyState[256];
	static BYTE preKeyState[256];

	static DIMOUSESTATE preMouseState;
	static DIMOUSESTATE mouseState;

	//Initializeをしてないとstatic関数を使えなくする
	static bool isInitialized_;

	HINSTANCE hInstance_ = nullptr;
	HWND hwnd_ = nullptr;

};
