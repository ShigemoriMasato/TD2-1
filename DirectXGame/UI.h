#pragma once
#include "Object/Object.h"
#include "Scene/Common/Scene.h"

class Button : public Object {
public:

	Button(Camera* camera, Vector3 center);
	~Button() = default;

	void Initialize() override;
	void Update() override;

	void SetSelected(bool isSelected) { isSelected_ = isSelected; }

private:

	bool isSelected_ = false;

};

class UI {
public:

	UI(Camera* camera);
	~UI() = default;

	void Update();
	void Draw();

	std::unique_ptr<Scene> GetSelectedScene(std::shared_ptr<CommonData> cd);

private:

	enum ButtonKind {
		BPMMasure,
		MotionGraph
	};

	std::vector<std::unique_ptr<Button>> buttons_;

	const int buttonNum_ = 3;
	int nowSelected_ = 0;

};

