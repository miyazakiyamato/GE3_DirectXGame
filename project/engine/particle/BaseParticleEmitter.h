#pragma once
#include "Matrix4x4.h"
#include <map>

class BaseParticleEmitter{
public:
	struct EmitterData {
		std::string textureFilePath;
		Transform transform;//発生中心
		uint32_t count = 3;//発生数
		float frequency = 0.5f;//発生頻度
		float frequencyTime = 0.0f;//頻度用時刻
		bool isEmitUpdate = false;//連続発生するか
	};;
public:
	BaseParticleEmitter() = default;
	virtual ~BaseParticleEmitter() = default;
	virtual void Initialize(const std::string& emitterName ) = 0;

	virtual void Update() = 0;
	//パーティクルの発生
	virtual void Emit() = 0;
protected:
	std::string name_ = ""; // エミッターの名前

	std::map<std::string, EmitterData> emitterDates_; // エミッターのデータを格納するマップ
public:
	/*const Vector3& GetPosition() { return transform_.translate; }
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
	void SetIsEmitUpdate(bool isEmitUpdate) { isEmitUpdate_ = isEmitUpdate; }*/
};

