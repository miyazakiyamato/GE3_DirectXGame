#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b0);

[numthreads(1024,1,1)]
void main(uint32_t3 DTid : SV_DispatchThreadID){
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles){
        if (gParticles[particleIndex].color.a != 0.0f){
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity * gPerFrame.deltaTime;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float32_t alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
    }
}