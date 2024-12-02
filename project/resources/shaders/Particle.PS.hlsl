#include "Particle.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = float32_t4(input.texcoord, 0.0f, 1.0f);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    output.color = input.color * textureColor;
    
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    
    return output;
}