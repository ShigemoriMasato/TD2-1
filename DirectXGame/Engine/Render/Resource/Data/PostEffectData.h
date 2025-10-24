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

enum class FadeType : int {
    Black = 0,      // ブラックフェード
 White = 1,// ホワイトフェード
    Radial = 2,     // 放射状フェード
    Wipe = 3,       // ワイプフェード
    Dissolve = 4,   // ディゾルブフェード
};

struct Fade {
    const std::string shaderFile = "Fade.PS.hlsl";
    float alpha = 0.0f;     // フェードの透明度 (0.0 - 1.0)
    FadeType type = FadeType::Black;  // フェードのタイプ
    Vector3 color = { 0.0f, 0.0f, 0.0f };  // フェードカラー（White時は{1,1,1}）
};

struct PostEffectData {
    Blur blur;
	Grayscale grayscale;
	Fade fade;
};
