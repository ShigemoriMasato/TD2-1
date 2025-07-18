#pragma once
#include "CommonData.h"
#include "../../Engine/Render/Render.h"
#include <memory>

class Scene {
public:
	Scene(std::shared_ptr<CommonData> commonData);
	~Scene() = default;

	virtual void Initialize() = 0;
	[[nodiscard]]
	virtual std::unique_ptr<Scene> Update() = 0;
	virtual void Draw() const = 0;

protected:

	std::shared_ptr<CommonData> commonData_;

};
