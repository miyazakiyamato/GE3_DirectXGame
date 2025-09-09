#include "Random.hlsli"
#include "../RandomFunctions.hlsli"

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};
struct Material{
    float32_t4x4 projectionInverse;
    float32_t depthSensitivity;
    float32_t threshold; // マスクの閾値
    float32_t3 edgeColor; // エッジの色
    float32_t time;
};
ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input){
    float32_t random = rand2dTo1d(input.texcoord * gMaterial.time);
    
    PixelShaderOutput output;
    output.color.rgb = gTexture.Sample(gSampler, input.texcoord).rgb * random;
    output.color.a = 1.0f;
    
    return output;
}