#include "BoxFilter.hlsli"

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const uint32_t kNumBoxX = 5;
static const uint32_t kNumBoxY = 5;

//static const float32_t2 kIndex3x3[3][3] = {
//    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
//    { { -1.0f,  0.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f } },
//    { { -1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f } },
//};
//static const float32_t kKernel3x3[3][3] = {
//    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
//    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
//    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
//};

float32_t3 convolution(VertexShaderOutput input){
    float32_t3 color = float32_t3(0.0f, 0.0f, 0.0f);
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    for (int32_t x = 0; x < kNumBoxX; ++x){
        for (int32_t y = 0; y < kNumBoxY; ++y){
            float32_t2 kIndex = float32_t2(x - int32_t(kNumBoxX / 2), y - int32_t(kNumBoxY / 2));
            float32_t kKernel = 1.0f / (kNumBoxX * kNumBoxY);
            
            float32_t2 texcoord = input.texcoord + kIndex * uvStepSize;
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            color.rgb += fetchColor.rgb * kKernel;
        }
    }
    
    return color;
}

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    output.color.rgb = convolution(input);
    return output;
}