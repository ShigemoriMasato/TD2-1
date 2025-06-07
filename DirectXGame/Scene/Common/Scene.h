#pragma once
#include "CommonData.h"
#include "../../Engine/Core/Renderer.h"

class Scene {
public:
	Scene(CommonData* commonData);
	~Scene() = default;

	[[nodiscard]]
	virtual Scene* Update() = 0;
	virtual void Render() = 0;

	void SetRenderer(Renderer* render) { renderer_ = render; }

protected:

	CommonData* commonData_;
	Renderer* renderer_ = nullptr;

};
