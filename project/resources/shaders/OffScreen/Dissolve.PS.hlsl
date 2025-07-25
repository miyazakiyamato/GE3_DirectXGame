#include "Dissolve.hlsli"

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};
struct Material{
    float32_t4x4 projectionInverse;
    float32_t depthSensitivity;
    float32_t threshold; // マスクの閾値
    float32_t3 edgeColor; // エッジの色
};
ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input){
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    // マスクが閾値以下の場合、透明にする
    if (mask <= gMaterial.threshold){discard;}
    
    PixelShaderOutput output;
    float32_t edge = 1.0f - smoothstep(gMaterial.threshold, gMaterial.threshold + 0.1f, mask);
    output.color.rgb = gTexture.Sample(gSampler,input.texcoord).rgb;
    output.color.rgb += edge * gMaterial.edgeColor;
    output.color.a = 1.0f;
    
    return output;
}