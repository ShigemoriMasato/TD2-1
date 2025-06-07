#pragma once
#include "MyMath.h"

class Camera;

class DebugCamera {
public:

	void Update();

private:

	Camera* camera = nullptr;

	Transform transform;

	Vector2 mousePosition = { 0.0f, 0.0f };
	Vector2 preMousePosition = { 0.0f, 0.0f };
};
