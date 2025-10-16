#include "MP.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTextures[8] : register(t0); // t0..t7 をまとめて使う
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTextures[input.textureIndex].Sample(gSampler, input.texcoord);
    output.color = textureColor * input.color;
    
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
