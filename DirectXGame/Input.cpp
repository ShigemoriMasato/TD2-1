#include "Input.h"
#include <cassert>
#include <algorithm>

void Input::Initialize() {
	HRESULT hr;
	hr = DirectInput8Create(
		hInstance_, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	//Keyboard
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	hr = keyboard_->SetCooperativeLevel(
		hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	//Mouse
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));

	hr = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));

	hr = mouse_->SetCooperativeLevel(
		hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

void Input::Update() {
	keyboard_->Acquire();

	std::memcpy(preKeyState, keyState, sizeof(keyState));

	keyboard_->GetDeviceState(sizeof(keyState), keyState);

	mouse_->Acquire();

	std::memcpy(&preMouseState, &mouseState, sizeof(DIMOUSESTATE));

	mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
}

BYTE Input::GetKeyState(int keyCode) {
	if (keyCode < 0 || keyCode >= 256) {
		return 0; // Invalid key code
	}
	return keyState[keyCode];
}

BYTE Input::GetPreKeyState(int keyCode) {
	if (keyCode < 0 || keyCode >= 256) {
		return 0; // Invalid key code
	}
	return preKeyState[keyCode];
}

Vector2 Input::GetMouseMove() {
	return Vector2(float(mouseState.lX), float(mouseState.lY));
}

BYTE* Input::GetMouseButtonState() {
	return reinterpret_cast<BYTE*>(&mouseState);
}

BYTE* Input::GetPreMouseButtonState() {
	return reinterpret_cast<BYTE*>(&preMouseState);
}
