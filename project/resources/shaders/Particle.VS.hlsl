#include "Particle.hlsli"

struct ParticleForGPU
{
    float32_t4x4 WVP; // ワールドビュー射影行列
    float32_t4x4 World; // ワールド行列
    float32_t4 color; // パーティクルの色
    float32_t4x4 uvTransform; // UV変換行列
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);

struct VertexShaderInput{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t4 color : COLOR0;
    float32_t4x4 uvTransform : UVTRANSFORM0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    // インスタンスごとのWVP行列を使用して位置を変換
    output.position = mul(input.position, gParticle[instanceId].WVP);
    // テクスチャ座標をそのまま渡す
    output.texcoord = input.texcoord;
    // インスタンスごとの色を使用
    output.color = gParticle[instanceId].color;
    //インスタンスごとのUVトランスフォームを使用
    output.uvTransform = gParticle[instanceId].uvTransform;
    
    return output;
}