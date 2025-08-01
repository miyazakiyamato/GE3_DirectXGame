#pragma once
#include "BaseParticleEmitter.h"
#include "DirectXCommon.h"

class ParticleManager;
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
	};
	struct PerFrame {
		float time; // 時間
		float deltaTime; // 1フレームの経過時間
	};
public:
	void Initialize(const std::string& emitterName) override;
	void Update() override;
	void Emit() override;
private:
	DirectXCommon* dxCommon_ = nullptr; // DirectXの共通インスタンス
	ParticleManager* particleManager_ = nullptr; // パーティクルマネージャーのインスタンス
	// エミッターのデータを格納するリソース
	ComPtr<ID3D12Resource> emitterDataResource_;
	EmitterData* emitterData_ = nullptr; // エミッターの球体情報
	//timeのリソース
	ComPtr<ID3D12Resource> perFrameResource_;
	PerFrame* perFrameData_ = nullptr; // フレームごとの時間情報

	std::string computeShaderPipelineName_ = "";
};

