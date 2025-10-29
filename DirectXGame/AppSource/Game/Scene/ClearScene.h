#pragma once
#include <Render/Resource/DrawResource.h>
#include <Camera/Camera.h>
#include <Resource/Texture/TextureManager.h>
#include <Render/Render.h>
#include "../NumberPlate.h"

struct ClearScene
{
	int score = 10000;
	float time = 0.0f;
	bool isGoal = false;

	std::unique_ptr<NumberPlate> timer = nullptr;

	void Initialize(TextureManager* textureManager);
	void Update(float deltaTime);
	void Draw(Render* render);

};

