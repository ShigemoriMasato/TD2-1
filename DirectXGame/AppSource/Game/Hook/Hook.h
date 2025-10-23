#pragma once
#include "../BaseObject.h"

class Hook :
    public BaseObject
{
public:
    static std::string TypeName() { return "Hook"; }
    void Initialize(ModelData* modelData, Camera* camera) override;
    void Update(float deltaTime) override;
    
};

