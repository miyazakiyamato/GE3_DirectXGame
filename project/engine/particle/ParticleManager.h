#pragma once
#include <map>
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
		Vector4 color;
	};
	struct MaterialData {
		std::string textureFilePath;
		uint32_t srvIndex;
	};
	struct Particle {
		Transform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
		//Matrix4x4 uvTransform;
	};
	struct ParticleGroup {
		MaterialData materialData;
		std::list<Particle> particles;
		uint32_t kNumInstance;
		uint32_t srvIndexForInstancing;
		//バッファリソース
		ComPtr<ID3D12Resource> vertexResource;
		ComPtr<ID3D12Resource> instancingResource;
		//バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		//バッファリソース内のデータを指すポインタ
		VertexData* vertexData = nullptr;
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
	//ブレンドモード変更
	void ChangeBlendMode(ParticleCommon::BlendMode blendMode);
	//パーティクルグループの生成
	void CreateParticleGroup(const std::string name, const std::string textureFilePath);
	//パーティクルの発生
	void Emit(const std::string name, const Vector3& position, uint32_t count);
private://ローカル関数
	//パーティクルの作成
	Particle CreateNewParticle(std::mt19937& randomEngine, const Vector3& position);
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
	//
	const uint32_t kParticleVertexNum = 4;
	const uint32_t kParticleIndexNum = 6;
	//バッファリソース
	ComPtr<ID3D12Resource> indexResource;
	//バッファリソース内のデータを指すポインタ
	uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//インスタンスの最大数
	uint32_t kMaxInstance = 100;

	//テクスチャサイズ
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	Vector2 textureSize_ = { 100.0f,100.0f };

	//デルタタイム
	const float kDeltaTime_ = 1.0f / 60.0f;
	//パーティクルデータ
	std::map<std::string, std::unique_ptr<ParticleGroup>> particleGroups;
public://ゲッターセッター

};
