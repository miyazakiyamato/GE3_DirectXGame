#pragma once
#include "Matrix4x4.h"
#include <map>
#include "ParticleManager.h"

class BaseParticleEmitter{
public:
	struct EmitterData {
		std::string textureFilePath = "plane";
		float frequency = 0.5f;//発生頻度
		float frequencyTime = 0.0f;//頻度用時刻
		bool isEmitUpdate = false;//連続発生するか
		ParticleManager::ParticleInitData particleInitData; // パーティクルの初期化データ
		uint32_t kDivide = 32; // 分割数
		float kOuterRadius = 1.0f; // 外側の半径
		float kInnerRadius = 0.1f; // 内側の半径
		float kTopRadius = 1.0f; // 上側の半径
		float kBottomRadius = 1.0f; // 下側の半径
		float kHeight = 3.0f; // 高さ
	};
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

	std::map<std::string, EmitterData> emitterDates_; // エミッターのデータを格納するマップ

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

