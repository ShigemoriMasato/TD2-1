#pragma once
#include "../Render/MyDirectX.h"
#include <memory>

class Scene;
struct CommonData;
class Input;
class Render;
class Sound;

class SceneManager final {
public:

	SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight);
	~SceneManager();
	void Update();
	void Draw() const;
	
private:

	Scene* scene_ = nullptr;
	Scene* nextScene_ = nullptr;

	std::shared_ptr<CommonData> commonData_ = nullptr;

	MyDirectX* myDirectX_ = nullptr;
	Render* render_ = nullptr;

	Input* input_ = nullptr;
	Sound* sound_ = nullptr;
};
