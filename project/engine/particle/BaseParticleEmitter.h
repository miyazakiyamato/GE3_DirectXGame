#pragma once
#include "Matrix4x4.h"
#include <map>
#include "ParticleManager.h"
#include "DirectXCommon.h"

class ParticleManager;
class SrvUavManager;
class GlobalVariables;
class BaseParticleEmitter{
public:
	// コンストラクタ/デストラクタ
	BaseParticleEmitter() = default;
	virtual ~BaseParticleEmitter() = default;
	// 初期化
	virtual void Initialize(const std::string& emitterName, uint32_t kMaxParticles);
	// 更新
	virtual void Update() = 0;
	//パーティクルの発生
	virtual void Emit() = 0;

	// 調整項目の初期化
	virtual void InitializeGlobalVariables();
	// 調整項目の適用
	virtual void ApplyGlobalVariables();
protected:
	DirectXCommon* dxCommon_ = nullptr; // DirectXの共通インスタンス
	ParticleManager* particleManager_ = nullptr; // パーティクルマネージャーのインスタンス
	SrvUavManager* srvUavManager_ = nullptr; // SRV/UAVマネージャーのインスタンス
	GlobalVariables* globalVariables_ = nullptr; // グローバル変数のインスタンス

	std::string name_ = ""; // エミッターの名前

	Transform transform_; // エミッターの変換情報

public:
	const std::string& GetName() const { return name_; }
	const Vector3& GetPosition() { return transform_.translate; }
	const Vector3& GetRotation() { return transform_.rotate; }
	const Vector3& GetSize() { return transform_.scale; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
	void SetSize(const Vector3& size) { transform_.scale = size; }
	void SetTexture(const std::string& textureName);
	void SetRing(const uint32_t& kDivide, const float& kOuterRadius, const float& kInnerRadius);
	void SetCylinder(const uint32_t& kDivide, const float& kTopRadius, const float& kBottomRadius, const float& kHeight);
};

