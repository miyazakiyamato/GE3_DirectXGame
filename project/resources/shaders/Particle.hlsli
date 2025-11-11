#pragma once
struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t4 color : COLOR0;
    float32_t3x3 uvTransform : UVTRANSFORM0;
};
struct Particle{
    uint32_t isBillboard; // ビルボードの有無
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
    float32_t2 uvTranslate;
    float32_t2 uvScale;
    float32_t3 startScale;
    float32_t3 endScale;
    float32_t4 startColor;
    float32_t4 endColor;
    float32_t2 startUvTranslate;
    float32_t2 endUvTranslate;
};
struct PerView{
    float32_t4x4 viewProjection;
    float32_t4x4 billboardMatrix;
};
struct PerFrame{
    float32_t time;
    float32_t deltaTime;
};
struct Limit{
    uint32_t kMaxParticles;
};