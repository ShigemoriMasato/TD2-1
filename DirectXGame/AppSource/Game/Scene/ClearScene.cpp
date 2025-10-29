#include "ClearScene.h"

void ClearScene::Initialize(TextureManager* textureManager)
{
	timer = std::make_unique<NumberPlate>();
	timer->Initialize(textureManager, 4, true);
}

void ClearScene::Update(float deltaTime)
{
	if (!isGoal)
		time += deltaTime;
	timer->Update(static_cast<int>(time));
}

void ClearScene::Draw(Render* render)
{
	timer->Draw(render);
}