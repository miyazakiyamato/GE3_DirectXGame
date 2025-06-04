#include "Vignette.hlsli"

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    //周囲を0に、中心になるほど明るくなるように調整
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    // correctだけで計算すると中心の最大値が0.0625で暗すぎるのでScaleで調整
    float vignette = correct.x * correct.y * 16.0f;//16倍
    vignette = saturate(pow(vignette,0.8f));//0.8乗
    output.color.rgb *= vignette;
    return output;
}