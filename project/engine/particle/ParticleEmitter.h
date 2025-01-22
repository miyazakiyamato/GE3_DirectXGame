#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class ParticleEmitter{
public:
	ParticleEmitter() {}

	void Initialize(const std::string name, const std::string textureFilePath);

	void Update();
	//パーティクルの発生
	void Emit();
private:
	std::string name_;
	std::string textureFilePath_;

	//発生中心
	Transform transform_{};
	//発生数
	uint32_t count_ = 3;
	//発生頻度
	float frequency_ = 0.5f;
	//頻度用時刻
	float frequencyTime_ = 0.0f;
	//連続発生するか
	bool isEmitUpdate_ = false;

	//デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;

public:
	const std::string& GetName() { return name_; }
	const Vector3& GetPosition() { return transform_.translate; }
	const Vector3& GetRotation() { return transform_.rotate; }
	const Vector3& GetSize() { return transform_.scale; }
	uint32_t GetCount() { return count_; }
	float GetFrequency() { return frequency_; }
	bool GetIsEmitUpdate() { return isEmitUpdate_; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
	void SetSize(const Vector3& size) { transform_.scale = size; }
	void SetCount(uint32_t count) { count_ = count; }
	void SetFrequency(float frequency) { frequency_ = frequency; }
	void SetIsEmitUpdate(bool isEmitUpdate) { isEmitUpdate_ = isEmitUpdate; }
};

