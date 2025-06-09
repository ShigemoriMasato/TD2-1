#pragma once
#include "Engine/Math/MyMath.h"
#include "Engine/Data/Transform.h"

class Player {
public:

	Player() = default;
	~Player() = default;

	void Initialize(Camera* camera);
	void Update();
	void Draw() const;

private:

	Transform transform_;
	Camera* camera_ = nullptr;

};

