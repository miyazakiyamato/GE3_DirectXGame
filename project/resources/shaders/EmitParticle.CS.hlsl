#include "Particle.hlsli"
#include "RandomFunctions.hlsli"

struct EmitterSphere{
    float32_t3 translate; // 位置
    float32_t radius; // 射出半径
    uint32_t count; // 射出数
    float32_t frequency; // 射出間隔
    float32_t frequencyTime; // 射出間隔調整用
    uint32_t emit; // 射出許可
    uint32_t isBillboard; // ビルボードの有無
    uint32_t isEmitUpdate; //連続発生するか
};

RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

[numthreads(1,1,1)]
void main(uint32_t3 DTid : SV_DispatchThreadID){
    if (gEmitter.emit != 0){
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex){
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if (0 <= freeListIndex && freeListIndex < kMaxParticles){
                int32_t particleIndex = gFreeList[freeListIndex];
                //gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].scale = float32_t3(1.0f,1.0f,1.0f);
                gParticles[particleIndex].translate = gEmitter.translate + mul(normalize(generator.Generate3d() - float32_t3(0.5f, 0.5f, 0.5f)), gEmitter.radius);
                gParticles[particleIndex].velocity = (generator.Generate3d() - 0.5f);
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].lifeTime = generator.Generate1d();
                gParticles[particleIndex].currentTime = 0;
                gParticles[particleIndex].isBillboard = gEmitter.isBillboard;
            }else{
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }
}