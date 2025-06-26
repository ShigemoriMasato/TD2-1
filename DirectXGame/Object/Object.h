#pragma once
#include "../Engine/Data/Transform.h"
#include "../Engine/Camera/Camera.h"

class Object {
public:

	Object(Camera* camera) : camera_(camera) {};
	virtual ~Object() = default;
	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw() const;

protected:

	Transform transform_{};
	uint32_t color = 0xffffffff;
	int handle_ = 1;

	Camera* camera_ = nullptr;
};

