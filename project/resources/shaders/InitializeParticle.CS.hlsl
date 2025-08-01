#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeCounter : register(u1);

[numthreads(1024,1,1)]
void main(uint32_t3 DTid : SV_DispatchThreadID){
    uint32_t particleIndex = DTid.x;
    if (particleIndex == 0){
        gFreeCounter[0] = 0;
    }
    if (particleIndex < kMaxParticles){
        gParticles[particleIndex] = (Particle) 0;
    }
}