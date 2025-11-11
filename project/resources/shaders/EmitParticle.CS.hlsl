#include "Particle.hlsli"
#include "RandomFunctions.hlsli"

struct EmitterSphere{
    float32_t3 translate; // 位置
    float32_t radius; // 射出半径
    uint32_t count; // 射出数
    float32_t frequency; // 射出間隔
    float32_t frequencyTime; // 射出間隔調整用
    uint32_t emit; // 射出許可
    float32_t4 startColor; // 開始色
    float32_t4 endColor; // 終了色
    uint32_t isBillboard; // ビルボードの有無
    uint32_t isEmitUpdate; //連続発生するか
    float32_t rlifeTimeMin; // random寿命最低値
    float32_t rlifeTimeMax; // random寿命最高値
    float32_t3 startScale; // スケール開始時の値
    float32_t3 endScale; // スケール終了時の値
    float32_t3 rVelocityMin; // random速度最低値
    float32_t3 rVelocityMax; // random速度最高値
    float32_t2 startUvTranslate; // uvTranslate開始時の値
    float32_t2 endUvTranslate; // uvTranslate終了時の値
};

RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);
ConstantBuffer<Limit> gLimit : register(b2);

[numthreads(1,1,1)]
void main(uint32_t3 DTid : SV_DispatchThreadID){
    if (gEmitter.emit != 0){
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex){
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if (0 <= freeListIndex && freeListIndex < gLimit.kMaxParticles){
                int32_t particleIndex = gFreeList[freeListIndex];
                gParticles[particleIndex].scale = gEmitter.startScale;
                gParticles[particleIndex].translate = gEmitter.translate + ((generator.Generate3d() * 2.0f - 1.0f) * gEmitter.radius);
                gParticles[particleIndex].velocity = gEmitter.rVelocityMin + (generator.Generate3d() * (gEmitter.rVelocityMax - gEmitter.rVelocityMin));
                gParticles[particleIndex].lifeTime = gEmitter.rlifeTimeMin + (generator.Generate1d() * (gEmitter.rlifeTimeMax - gEmitter.rlifeTimeMin));
                gParticles[particleIndex].currentTime = 0;
                gParticles[particleIndex].isBillboard = gEmitter.isBillboard;
                gParticles[particleIndex].uvTranslate = float32_t2(0.0f,0.0f);
                gParticles[particleIndex].uvScale = float32_t2(1,1);
                gParticles[particleIndex].startScale = gEmitter.startScale;
                gParticles[particleIndex].endScale = gEmitter.endScale;
                gParticles[particleIndex].color = gEmitter.startColor;
                gParticles[particleIndex].startColor = gEmitter.startColor;
                gParticles[particleIndex].endColor = gEmitter.endColor;
                gParticles[particleIndex].startUvTranslate = gEmitter.startUvTranslate;
                gParticles[particleIndex].startUvTranslate = gEmitter.endUvTranslate;
                
            }else{
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
        }
    }
}