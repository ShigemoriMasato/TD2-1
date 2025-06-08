#include "Input.h"
#include <cassert>
#include <algorithm>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

DIMOUSESTATE Input::mouseState = {};
DIMOUSESTATE Input::preMouseState = {};
BYTE Input::keyState[256] = {};
BYTE Input::preKeyState[256] = {};
bool Input::isInitialized_ = false;

void Input::Initialize() {
	if (isInitialized_) {
		return; // Already initialized
	}

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
	assert(SUCCEEDED(hr));

	isInitialized_ = true;
}

void Input::Update() {
	keyboard_->Acquire();

	std::memcpy(preKeyState, keyState, sizeof(keyState));

	keyboard_->GetDeviceState(sizeof(keyState), keyState);

	mouse_->Acquire();

	std::memcpy(&preMouseState, &mouseState, sizeof(DIMOUSESTATE));

	mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
}

BYTE* Input::GetKeyState() {
	if (!isInitialized_) {
		return nullptr;
	}
	return keyState;
}

BYTE* Input::GetPreKeyState() {
	if(!isInitialized_) {
		return nullptr;
	}
	return preKeyState;
}

BYTE Input::GetKeyState(int keyCode) {
	if (!isInitialized_) {
		return {};
	}
	if (keyCode < 0 || keyCode >= 256) {
		return {}; // Invalid key code
	}
	return keyState[keyCode];
}

BYTE Input::GetPreKeyState(int keyCode) {
	if (!isInitialized_) {
		return {};
	}
	if (keyCode < 0 || keyCode >= 256) {
		return {}; // Invalid key code
	}
	return preKeyState[keyCode];
}

Vector2 Input::GetMouseMove() {
	if (!isInitialized_) {
		return {};
	}
	return Vector2(float(mouseState.lX), float(mouseState.lY));
}

float Input::GetMouseWheel() {
	if(!isInitialized_) {
		return 0.0f;
	}
	return float(mouseState.lZ);
}

BYTE* Input::GetMouseButtonState() {
	if (!isInitialized_) {
		return nullptr;
	}
	return mouseState.rgbButtons;
}

BYTE* Input::GetPreMouseButtonState() {
	if (!isInitialized_) {
		return nullptr;
	}
	return reinterpret_cast<BYTE*>(&preMouseState);
}
