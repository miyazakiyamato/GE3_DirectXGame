#include "DepthBasedOutline.hlsli"

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};
struct Material{
    //uint32_t kNumBoxX = 3;
    //uint32_t kNumBoxY = 3;
    float32_t4x4 projectionInverse;
    float32_t depthSensitivity;
};
ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

Texture2D<float32_t> gDepthTexture : register(t1);
SamplerState gSamplerPoint : register(s1);

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
            float32_t ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
            float32_t4 viewSpace = mul(float32_t4(0.0f,0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
            float32_t viewZ = viewSpace.z * rcp(viewSpace.w); //同時座標系からデカルト座標系へ変換
            difference.x += viewZ * kPrewittHorizontalKernel[x][y];
            difference.y += viewZ * kPrewittVerticalKernel[x][y];
        }
    }
    
    float32_t weight = length(difference);
    weight = saturate(weight * gMaterial.depthSensitivity);
    
    PixelShaderOutput output;
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
}