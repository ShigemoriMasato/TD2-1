#pragma once
#include <Transform/Vector.h>
#include <string>

//今なんか挙動がちょっとおかしい
struct Blur {
    const std::string shaderFile = "Blur.PS.hlsl";

    float intensity = 0.0f; // ブラー強度
    float kernelSize = 0.0f; // カーネルサイズ
};

struct Grayscale {
    const std::string shaderFile = "Grayscale.PS.hlsl";

    float intensity = 0.0f; // グレースケール強度
};

struct Fade {
    const std::string shaderFile = "Fade.PS.hlsl";
    float alpha = 0.0f; // フェードの透明度
};

struct PostEffectData {
    Blur blur;
	Grayscale grayscale;
	Fade fade;
};
