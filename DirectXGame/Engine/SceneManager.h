#pragma once
#include "Core/MyDirectX.h"

class Scene;
struct CommonData;
class Input;
class Renderer;

class SceneManager final {
public:

	SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight);
	~SceneManager();
	void Update();
	void Render();
	
private:

	Scene* scene_ = nullptr;
	Scene* nextScene_ = nullptr;

	CommonData* commonData_ = nullptr;

	MyDirectX* myDirectX_ = nullptr;
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;
};
