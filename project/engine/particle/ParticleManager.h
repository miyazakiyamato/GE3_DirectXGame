#pragma once
#include <map>
#include "BlendMode.h"
#include "random"
#include "DirectXCommon.h"
#include "Matrix4x4.h"

class DirectXCommon;
class SrvUavManager;
class ParticleManager {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};
	struct MaterialData {
		std::string textureFilePath = "circle.png";
		uint32_t srvIndex;
	};
	struct Particle {
		//Transform transform;
		//Transform uvTransform;
		Vector3 translate;
		Vector3 scale;
		float lifeTime;
		Vector3 velocity;
		float currentTime;
		Vector4 color;
	};
	struct PerView{
		Matrix4x4 viewProjection;
		Matrix4x4 billboardMatrix;
	};
	
	struct ParticleGroup {
		MaterialData materialData;
		ComPtr<ID3D12Resource> particleResource; // パーティクルデータ用リソース
		//デスクリプタハンドル
		uint32_t particleSrvIndex; // SRV (描画用)
		uint32_t particleUavIndex; // UAV (計算用)
		//WorldViewProjection用のリソース
		ComPtr<ID3D12Resource> perViewResource; // PerView定数バッファ
		PerView* perViewData = nullptr; // マッピング用ポインタ
		//freeCounter
		ComPtr<ID3D12Resource> freeCounterResource;
		uint32_t freeCounterUAVIndex; // freeCounterのインデックス
		//頂点
		uint32_t kParticleVertexNum;
		uint32_t kParticleIndexNum;
		ComPtr<ID3D12Resource> vertexResource;
		ComPtr<ID3D12Resource> indexResource;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		VertexData* vertexData = nullptr;
		uint32_t* indexData = nullptr;
		//State
		BlendMode blendMode_ = BlendMode::kAdd;
		std::string pipelineStateName = "";
		Vector2 textureLeftTop_ = { 0.0f,0.0f };
		Vector2 textureSize_ = { 100.0f,100.0f };
	};

public://メンバ関数
	//シングルトンインスタンスの取得
	static ParticleManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon, SrvUavManager* srvUavManager);
	//終了
	void Finalize();
	//更新
	void Update();
	//描画
	void Draw();
	//パーティクルグループの生成
	void CreateParticleGroup(const std::string name);
	/*void CreateRingParticleGroup(const std::string name,
		const uint32_t& kDivide,const float& kOuterRadius,const float& kInnerRadius);
	void CreateCylinderParticleGroup(const std::string name,
		const uint32_t& kDivide, const float& kTopRadius, const float& kBottomRadius,const float& kHeight);
	*/
	//パーティクルの発生
	void Emit(const std::string name, const Vector3& position, uint32_t count);

	//調整項目の更新
	//void UpdateGlobalVariables();
private://ローカル関数
	void CreateParticle(ParticleGroup* group);
	void CreatePlane(ParticleGroup* group);
	//調整項目の初期化
	void InitializeGlobalVariables();
	// 調整項目の適用
	void ApplyGlobalVariables();
private://シングルインスタンス
	static ParticleManager* instance;

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;
private://メンバ変数
	//ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	SrvUavManager* srvUavManager_ = nullptr;
	//インスタンスの最大数
	uint32_t kMaxParticles = 1024;

	std::string initCSPipelineName_ = "";
	std::string updateCSPipelineName_ = "";
	//ランダムエンジン
	std::mt19937 randomEngine_;

	std::string groupNameText = ""; // グループ名
	char buffer[128] = ""; // 入力用のバッファ
	std::string typeNameText = ""; // タイプ名
	char buffer2[128] = ""; // 入力用のバッファ

	//パーティクルグループを作るデータ
	//std::map<std::string, std::unique_ptr<ParticleGroupCreateData>> particleGroupCreateDates_;
	//パーティクルデータ
	std::map<std::string, std::unique_ptr<ParticleGroup>> particleGroups;
public://ゲッターセッター
	DirectXCommon* GetDirectXCommon() { return dxCommon_; }
	SrvUavManager* GetSrvUavManager() { return srvUavManager_; }
	std::map<std::string, std::unique_ptr<ParticleGroup>>& GetParticleGroups() { return particleGroups; }
	ParticleGroup* GetParticleGroup(std::string name);
	const BlendMode& GetBlendMode(std::string name) { return particleGroups[name]->blendMode_; }

	void SetBlendMode(std::string name, BlendMode blendMode);
};
