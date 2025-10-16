#pragma once
#include "Matrix4x4.h"
#include <map>
#include "ParticleManager.h"

class BaseParticleEmitter{
public:
	BaseParticleEmitter() = default;
	virtual ~BaseParticleEmitter() = default;
	virtual void Initialize(const std::string& emitterName );

	virtual void Update();
	//パーティクルの発生
	virtual void Emit();

	//調整項目の更新
	void UpdateGlobalVariables();
private://ローカル関数
	//調整項目の初期化
	void InitializeGlobalVariables();
	// 調整項目の適用
	void ApplyGlobalVariables();
protected:
	std::string name_ = ""; // エミッターの名前

	Transform transform_; // エミッターの変換情報

	std::string groupNameText = ""; // グループ名
	char buffer[128] = ""; // 入力用のバッファ
	std::string typeNameText = ""; // タイプ名
	char buffer2[128] = ""; // 入力用のバッファ
public:
	const Vector3& GetPosition() { return transform_.translate; }
	const Vector3& GetRotation() { return transform_.rotate; }
	const Vector3& GetSize() { return transform_.scale; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
	void SetSize(const Vector3& size) { transform_.scale = size; }
};

