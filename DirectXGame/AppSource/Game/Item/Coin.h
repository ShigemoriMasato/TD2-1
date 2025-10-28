#pragma once
#include "../BaseObject.h"

class Coin :
    public BaseObject
{
public:
    static std::string TypeName(){return "Coin";}

    void Initialize(ModelData* modelData, Camera* camera) override;
    void Update(float deltaTime) override;
    void OnCollision(BaseObject* other) override;

private:
    int soundHandle_ = -1;
};

