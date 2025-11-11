#pragma once
#include "BaseParticleEmitter.h"

class EmitterSphere : public BaseParticleEmitter{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
	struct EmitterData {
		Vector3 translate; // 位置
		float radius; // 射出半径
		uint32_t count; // 射出数
		float frequency; // 射出間隔
		float frequencyTime; // 射出間隔調整用
		uint32_t emit; // 射出許可
		Vector4 startColor; // 開始色
		Vector4 endColor; // 終了色
		uint32_t isBillboard = 1; // ビルボードの有無
		uint32_t isEmitUpdate = 0;//連続発生するか
		float rlifeTimeMin; // random寿命最低値
		float rlifeTimeMax; // random寿命最高値
		Vector3 startScale; // スケール開始時の値
		float pad0;
		Vector3 endScale; // スケール終了時の値
		float pad1;
		Vector3 rVelocityMin; // random速度最低値
		float pad2;
		Vector3 rVelocityMax; // random速度最高値
		float pad3;
		Vector2 startUvTranslate; // uvTranslate開始時の値
		Vector2 endUvTranslate; // uvTranslate終了時の値
	};
	struct PerFrame {
		float time; // 時間
		float deltaTime; // 1フレームの経過時間
		float pad[2];
	};
public:
	void Initialize(const std::string& emitterName, uint32_t kMaxParticles) override;
	void Update() override;
	void Emit() override;
	void InitializeGlobalVariables() override;
	void ApplyGlobalVariables() override;
private:
	// エミッターのデータを格納するリソース
	ComPtr<ID3D12Resource> emitterDataResource_;
	EmitterData* emitterData_ = nullptr; // エミッターの球体情報

	std::string computeShaderPipelineName_ = "";
public:
	// ゲッター
	const Vector3& GetTranslate() const { return emitterData_->translate; }
	const float& GetRadius() const { return emitterData_->radius; }
	const uint32_t& GetCount() const { return emitterData_->count; }
	const float& GetFrequency() const { return emitterData_->frequency; }
	bool GetIsBillboard() const { return (bool)emitterData_->isBillboard; }
	bool GetIsEmitUpdate() const { return (bool)emitterData_->isEmitUpdate; }
	// セッター
	void SetTranslate(const Vector3& translate) { emitterData_->translate = translate; }
	void SetRadius(float radius) { emitterData_->radius = radius; }
	void SetCount(uint32_t count) { emitterData_->count = count; }
	void SetFrequency(float frequency) { emitterData_->frequency = frequency; }
	void SetIsBillboard(bool isBillboard) { emitterData_->isBillboard = isBillboard; }
	void SetIsEmitUpdate(bool isEmitUpdate) { emitterData_->isEmitUpdate = isEmitUpdate; }
	void SetRLifeTimeMin(float rlifeTimeMin) { emitterData_->rlifeTimeMin = rlifeTimeMin; }
	void SetRLifeTimeMax(float rlifeTimeMax) { emitterData_->rlifeTimeMax = rlifeTimeMax; }
	void SetRVelocityMin(const Vector3& rVelocityMin) { emitterData_->rVelocityMin = rVelocityMin; }
	void SetRVelocityMax(const Vector3& rVelocityMax) { emitterData_->rVelocityMax = rVelocityMax; }
};

