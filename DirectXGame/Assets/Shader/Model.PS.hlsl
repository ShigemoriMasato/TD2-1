#include "Model.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    float shininess;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    output.color = textureColor * gMaterial.color;
    
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
