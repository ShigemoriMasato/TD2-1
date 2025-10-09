#pragma once
#include "../BaseObject.h"

#include <Camera/DebugCamera.h>
#include <Scene/Data/BaseScene.h>
#include <unordered_map>

class GameScene :
    public BaseScene
{
public:

    void Initialize() override;
    std::unique_ptr<BaseScene> Update() override;
    void Draw() override;

private:
    std::vector<std::unique_ptr<BaseObject>> objects_;
    std::unique_ptr<DebugCamera> camera_ = nullptr;

    std::unordered_map<Key, bool> keys_{};
};

