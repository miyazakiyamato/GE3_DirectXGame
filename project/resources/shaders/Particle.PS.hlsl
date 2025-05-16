#include "Particle.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t alphaReference = 0.0f;

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //テクスチャUV
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), input.uvTransform);
    //テクスチャカラー
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    output.color = input.color * textureColor;
    
    if (output.color.a == alphaReference || output.color.a == 0.0f){
        discard;
    }
    
    return output;
}