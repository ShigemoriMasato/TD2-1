#include "Object3d.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 Local;
};

StructuredBuffer<TransformationMatrix> gTransformMatrix : register(t0);

cbuffer BoneMatrix : register(b0)
{
    float4x4 boneMatrix[128];
};

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    int nodeIndex : NODEINDEX0;
    int4 boneIndices : BONEINDEX0;
    float4 boneWeights : BONEWEIGHT0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    
    float4 localPos = input.position;
    
    float totalWeight = input.boneWeights.x + input.boneWeights.y + input.boneWeights.z + input.boneWeights.w;
    
    if (totalWeight > 0.001f)
    {
        float4 skinnedPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 4; ++i) 
        {
            int boneIndex = input.boneIndices[i];
            
            if (boneIndex >= 0) 
            {
                skinnedPos += mul(localPos, boneMatrix[boneIndex]) * input.boneWeights[i];
            }
        }
        localPos = skinnedPos;
    }
    else
    {
        localPos = mul(localPos, gTransformMatrix[input.nodeIndex].Local);
    }
    
    output.position = mul(localPos, gTransformMatrix[input.nodeIndex].WVP);
    
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformMatrix[input.nodeIndex].World));
    return output;
}
