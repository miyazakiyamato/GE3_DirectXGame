#include "Particle.hlsli"

StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

struct VertexShaderInput{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID){
    VertexShaderOutput output;
    Particle particle = gParticle[instanceId];
    float32_t4x4 worldMatrix = gPerView.billboardMatrix;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    output.position = mul(input.position, mul(worldMatrix,gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;
    
    return output;
}