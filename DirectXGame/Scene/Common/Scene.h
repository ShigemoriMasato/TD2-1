#pragma once
#include "CommonData.h"
#include "../../Engine/Core/Render.h"

class Scene {
public:
	Scene(CommonData* commonData);
	~Scene() = default;

	[[nodiscard]]
	virtual Scene* Update() = 0;
	virtual void Draw() const = 0;

	void SetRenderer(Render* render) { render_ = render; }

protected:

	CommonData* commonData_;
	Render* render_ = nullptr;

};
