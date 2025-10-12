#include "Model.hlsli"

cbuffer Material : register(b0)
{
    float4 color;
    float4x4 uvTransform;
    float shininess;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    output.color = textureColor * color;
    
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
