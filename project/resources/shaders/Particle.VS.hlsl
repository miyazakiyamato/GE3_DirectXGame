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
    float32_t4x4 worldMatrix = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    
    if (particle.isBillboard == 1){
        worldMatrix = gPerView.billboardMatrix;
    }
    
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    output.position = mul(input.position, mul(worldMatrix,gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;
    output.uvTransform = float32_t3x3(
        particle.uvScale.x, 0, 0,
        0, particle.uvScale.y, 0,
        particle.uvTranslate.x, particle.uvTranslate.y, 1
    );
    
    return output;
}