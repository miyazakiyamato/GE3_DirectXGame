#pragma once
#include <map>
#include "Particle.h"
#include "ParticleCommon.h"
#include "random"

class DirectXCommon;
class SrvManager;
class ParticleManager {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
	struct MaterialData {
		std::string textureFilePath;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU; // テクスチャのGPUハンドル
	};
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};
	struct ParticleGroup
	{
		MaterialData materialData;
		std::list<Particle> particles;
		uint32_t srvIndexForInstancing;
		ComPtr<ID3D12Resource> instancingResource;
		uint32_t kNumInstance;
		ParticleForGPU* instancingData = nullptr;
	};
public://メンバ関数
	//シングルトンインスタンスの取得
	static ParticleManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
	//終了
	void Finalize();
	//更新
	void Update();
	//描画
	void Draw();
	//パーティクルグループの生成
	void CreateParticleGroup(const std::string name, const std::string textureFilePath);
	//パーティクルの発生
	void Emit(const std::string name, const Vector3& position, uint32_t count);
private://シングルインスタンス
	static ParticleManager* instance;

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;
private://メンバ変数
	//ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	std::unique_ptr<ParticleCommon> particleCommon_;
	//パーティクルデータ
	std::map<std::string, std::unique_ptr<ParticleGroup>> particleGroups;
public://ゲッターセッター

};
