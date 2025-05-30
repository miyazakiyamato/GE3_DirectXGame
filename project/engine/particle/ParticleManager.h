#pragma once
#include <map>
#include "BlendMode.h"
#include "random"
#include "DirectXCommon.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

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
		//Vector3 normal;
		Vector4 color;
	};
	struct MaterialData {
		std::string textureFilePath;
		uint32_t srvIndex;
	};
	struct Particle {
		Transform transform;
		Transform uvTransform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
		Matrix4x4 uvTransform;
	};
	struct ParticleInitData {
		Vector3 randomScaleMax{1.0f,1.0f,1.0f};
		Vector3 randomScaleMin{1.0f,1.0f,1.0f};
		Vector3 randomRotateMax{};
		Vector3 randomRotateMin{};
		Vector3 randomVelocityMax{1.0f,1.0f,1.0f};
		Vector3 randomVelocityMin{-1.0f,-1.0f,-1.0f};
		Vector4 randomColorMax{1.0f,1.0f,1.0f,1.0f};
		Vector4 randomColorMin{0.0f,0.0f,0.0f,1.0f};
		float lifeTime = 2.0f;
		bool isBillboard = true;
	};
	struct ParticleGroupCreateData {
		std::string name = "";
		std::string textureFilePath = "circle2.png";
		std::string particleType = "plane";
		ParticleInitData particleInitData;
	};
	struct ParticleGroup {
		ParticleInitData particleInitData;
		std::list<Particle> particles;//パーティクルのリスト
		MaterialData materialData;//マテリアルデータ
		//インスタンスの数
		uint32_t kParticleVertexNum;
		uint32_t kParticleIndexNum;
		uint32_t kNumInstance = 0;
		uint32_t srvIndexForInstancing;
		//バッファリソース
		ComPtr<ID3D12Resource> vertexResource;
		ComPtr<ID3D12Resource> indexResource;
		ComPtr<ID3D12Resource> instancingResource;
		//バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		//バッファリソース内のデータを指すポインタ
		VertexData* vertexData = nullptr;
		uint32_t* indexData = nullptr;
		ParticleForGPU* instancingData = nullptr;
		BlendMode blendMode_ = BlendMode::kAdd;
		std::string pipelineStateName_ = "";
		//テクスチャサイズ
		Vector2 textureLeftTop_ = { 0.0f,0.0f };
		Vector2 textureSize_ = { 100.0f,100.0f };
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
	void CreateParticleGroup(const std::string name);
	void CreateRingParticleGroup(const std::string name,
		const uint32_t& kDivide,const float& kOuterRadius,const float& kInnerRadius);
	void CreateCylinderParticleGroup(const std::string name,
		const uint32_t& kDivide, const float& kTopRadius, const float& kBottomRadius,const float& kHeight);
	//パーティクルの発生
	void Emit(const std::string name, const Vector3& position, uint32_t count);

	//調整項目の更新
	void UpdateGlobalVariables();
private://ローカル関数
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
	SrvManager* srvManager_ = nullptr;
	//インスタンスの最大数
	uint32_t kMaxInstance = 1000;

	//ランダムエンジン
	std::mt19937 randomEngine_;

	std::string groupNameText = ""; // グループ名
	char buffer[128] = ""; // 入力用のバッファ
	std::string typeNameText = ""; // タイプ名
	char buffer2[128] = ""; // 入力用のバッファ

	//パーティクルグループを作るデータ
	std::map<std::string, std::unique_ptr<ParticleGroupCreateData>> particleGroupCreateDates_;
	//パーティクルデータ
	std::map<std::string, std::unique_ptr<ParticleGroup>> particleGroups;
public://ゲッターセッター
	std::map<std::string, std::unique_ptr<ParticleGroup>>& GetParticleGroups() { return particleGroups; }
	ParticleGroup* GetParticleGroup(std::string name);
	const BlendMode& GetBlendMode(std::string name) { return particleGroups[name]->blendMode_; }

	void SetBlendMode(std::string name, BlendMode blendMode);
};
