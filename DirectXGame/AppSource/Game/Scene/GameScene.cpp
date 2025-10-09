#include "GameScene.h"
#include "../Player/Player.h"

void GameScene::Initialize()
{
    camera_ = std::make_unique<DebugCamera>();
    camera_->Initialize();

    //プレイヤー初期化
    {
        auto player = std::make_unique<Player>();
        auto handle = modelManager_->LoadModel("Bunny");
        player->Initialize(modelManager_->GetModelData(handle), camera_.get());
        objects_.push_back(std::move(player));
    }

}

std::unique_ptr<BaseScene> GameScene::Update()
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

void GameScene::Draw()
{
    render_->PreDraw();
    for (auto& object : objects_)
    {
        object->Draw(render_);
    }
}
