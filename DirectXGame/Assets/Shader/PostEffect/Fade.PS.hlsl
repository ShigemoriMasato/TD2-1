#include "PostEffect.hlsli"

cbuffer FadeParameters : register(b0)
{
    float3 padding;
    float FadeAmount; // 0.0 = no fade, 1.0 = full fade
}

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // テクスチャから色をサンプリング
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // フェードアウト: FadeAmount が 1.0 に近づくほど黒くなる
    // FadeAmount = 0.0 -> 元の色
    // FadeAmount = 1.0 -> 完全に黒
    float3 fadeColor = lerp(textureColor.rgb, float3(0.0, 0.0, 0.0), FadeAmount);
    
    // アルファ値は保持
    output.color = float4(fadeColor, textureColor.a);
    
    return output;
}