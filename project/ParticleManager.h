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
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 color;
	};
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

	//バッファリソース
	ComPtr<ID3D12Resource> vertexResource;
	ComPtr<ID3D12Resource> indexResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//アンカーポイント
	Vector2 anchorPoint_ = { 0.5f,0.5f };
	//テクスチャサイズ
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	Vector2 textureSize_ = { 100.0f,100.0f };

	//パーティクルデータ
	std::map<std::string, std::unique_ptr<ParticleGroup>> particleGroups;
public://ゲッターセッター

};
