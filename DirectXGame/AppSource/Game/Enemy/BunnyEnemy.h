#pragma once
#include "BaseEnemy.h"

class BunnyEnemy : public BaseEnemy
{
public:
    BunnyEnemy();
    ~BunnyEnemy() override = default;

    void Initialize(ModelData* modelData, Camera* camera) override;
    void Update() override;
    void Draw(Render* render) override;

private:
    float rotationSpeed_ = 0.5f;
};