#include "Particle.hlsli"

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;

    // インスタンスごとのUV変換を適用
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), input.uvTransform);
    // テクスチャサンプル
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    // 色の計算 (テクスチャ色とインスタンスの色を掛け合わせる)
    output.color = textureColor * input.color;
    // アルファ値が0の場合は破棄
    if (textureColor.a == 0.0f) {
        discard;
    }

    return output;
}