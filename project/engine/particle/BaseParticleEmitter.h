#pragma once
#include "Matrix4x4.h"
#include <map>

class BaseParticleEmitter{
public:
	BaseParticleEmitter() = default;
	virtual ~BaseParticleEmitter() = default;
	virtual void Initialize(const std::string& emitterName ) = 0;

	virtual void Update() = 0;
	//パーティクルの発生
	virtual void Emit() = 0;
protected:
	std::string name_ = ""; // エミッターの名前

	Transform transform_; // エミッターの変換情報
public:
	const Vector3& GetPosition() { return transform_.translate; }
	const Vector3& GetRotation() { return transform_.rotate; }
	const Vector3& GetSize() { return transform_.scale; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
	void SetSize(const Vector3& size) { transform_.scale = size; }
};

