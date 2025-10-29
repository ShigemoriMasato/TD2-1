#pragma once
#include "../BaseObject.h"

class Coin :
    public BaseObject
{
public:
    Coin();
    ~Coin();

    static std::string TypeName(){return "Coin";}

    void Initialize(ModelData* modelData, Camera* camera) override;
    void Update(float deltaTime) override;
    void OnCollision(BaseObject* other) override;

    static int GetScore(){ return score; }

    static void ResetScore() { score = 0; }
private:
    int soundHandle_ = -1;
    static inline int score = 0;
};

