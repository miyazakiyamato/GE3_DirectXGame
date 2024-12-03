#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class ParticleEmitter{
public:
	ParticleEmitter() {}

	void Initialize();

	void Update();
	//パーティクルの発生
	void Emit();
private:
	Transform transform{};
	//発生数
	uint32_t count_ = 3;
	//発生頻度
	float frequency_ = 0.5f;
	//頻度用時刻
	float frequencyTime_ = 0.0f;

	//デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;
};

