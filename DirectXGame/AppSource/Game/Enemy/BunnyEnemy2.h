#pragma once
#include "BaseEnemy.h"

class BunnyEnemy2 : public BaseEnemy
{
public:
    BunnyEnemy2();
    ~BunnyEnemy2() override = default;

    void Initialize(ModelData* modelData, Camera* camera) override;
    void Update(float deltaTime) override;
    void Draw(Render* render) override;

private:
    // 回転速度（右回転）
    float rotationSpeed_ = 0.5f;
};