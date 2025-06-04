#include "LuminanceBasedOutline.hlsli"

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const uint32_t kNumBoxX = 3;
static const uint32_t kNumBoxY = 3;
static const float32_t2 uvStepSize = float32_t2(1.0f / 1024.0f, 1.0f / 768.0f);

static const float32_t kPrewittHorizontalKernel[3][3] = {
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};
static const float32_t kPrewittVerticalKernel[3][3] = {
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};
float32_t Luminance(float32_t3 v){
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input){
    float32_t2 kIndex[kNumBoxX][kNumBoxY];
    for (int32_t x = 0; x < kNumBoxX; ++x){
        for (int32_t y = 0; y < kNumBoxY; ++y){
            kIndex[x][y] = float32_t2(x - int32_t(kNumBoxX / 2), y - int32_t(kNumBoxY / 2));
        }
    }
    
    float32_t2 difference = float32_t2(0.0f, 0.0f);
    for (int32_t x = 0; x < kNumBoxX; ++x){
        for (int32_t y = 0; y < kNumBoxY; ++y){
            float32_t2 texcoord = input.texcoord + kIndex[x][y] * uvStepSize;
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            float32_t luminance = Luminance(fetchColor);
            difference.x += luminance * kPrewittHorizontalKernel[x][y];
            difference.y += luminance * kPrewittVerticalKernel[x][y];
        }
    }
    
    float32_t weight = length(difference);
    weight = saturate(weight * 6.0f);
    
    PixelShaderOutput output;
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler,input.texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
}