#include "EnemyTestScene.h"

void EnemyTestScene::Initialize()
{

	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

}

std::unique_ptr<BaseScene> EnemyTestScene::Update()
{
	keys_ = commonData->keyManager_->GetKeyStates();
	camera_->Update();

	//オブジェクト更新
	for (auto& object : objects_)
	{
		object->Update();
	}

	return nullptr;
}

void EnemyTestScene::Draw()
{
	render_->PreDraw();
	for (auto& object : objects_)
	{
		object->Draw(render_);
	}
}
