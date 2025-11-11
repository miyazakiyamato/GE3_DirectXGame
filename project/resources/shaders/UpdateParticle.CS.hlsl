#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);
ConstantBuffer<Limit> gLimit : register(b1);



[numthreads(1024,1,1)]
void main(uint32_t3 DTid : SV_DispatchThreadID){
    uint32_t particleIndex = DTid.x;
    if (particleIndex < gLimit.kMaxParticles){
        gParticles[particleIndex].translate += gParticles[particleIndex].velocity * gPerFrame.deltaTime;
        gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
        float32_t t = saturate(gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
        float32_t4 startColor = float32_t4(1,1,1,1);
        float32_t4 endColor = float32_t4(1,1,1,0);
        gParticles[particleIndex].color = startColor + (endColor - startColor) * t;
        
        if (gParticles[particleIndex].lifeTime <= gParticles[particleIndex].currentTime){
            gParticles[particleIndex].scale = (float32_t3) 0;
            gParticles[particleIndex].color.a = 0.0f;
            gParticles[particleIndex].currentTime = 0.0f;
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            if ((freeListIndex + 1) < gLimit.kMaxParticles){
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else{
                //ここに来るはずがないが安全策
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}