#pragma once
#include <Transform/Vector.h>
#include <string>

struct Blur {
    const std::string shaderFile = "Blur.PS.hlsl";

    float intensity; // ブラー強度
    float kernelSize; // カーネルサイズ
};

struct PostEffectData {
    Blur blur;
};
