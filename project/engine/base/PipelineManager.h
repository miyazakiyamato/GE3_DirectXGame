#pragma once
#include "DirectXCommon.h"
#include "BlendMode.h"
#include "BasePipeline.h"
#include <map>

enum class CullMode {
	kNone,      //!< カリングなし
	kFront,     //!< フロントカリング
	kBack,      //!< バックカリング
};
enum class FillMode {
	kSolid,     //!< ソリッド描画
	kWireframe, //!< ワイヤーフレーム描画
};

struct PipelineState {
	std::string shaderName = "";
	BlendMode blendMode = BlendMode::kNone;
	CullMode cullMode = CullMode::kBack; // カリングモード
	FillMode fillMode = FillMode::kSolid; // 塗りつぶしモード
	bool depthEnable = true; // 深度テストを有効にするか
	bool isOffScreen = false; // オフスクリーン描画かどうか
};

class PipelineManager {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
	struct PipelineData {
		PipelineState state;
		ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	};
public://メンバ関数
	//シングルトンインスタンスの取得
	static PipelineManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//終了
	void Finalize();
	//描画設定
	void DrawSetting(const std::string& stateName);
	//グラフィックスパイプラインの生成
	void CreatePipelineState(const std::string& stateName, const PipelineState& pipelineState);
private:
	//ルートシグネチャの作成
	void CreateRootSignature(PipelineData& pipeline);
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline(PipelineData& pipeline);
private://シングルインスタンス
	static PipelineManager* instance;

	PipelineManager() = default;
	~PipelineManager() = default;
	PipelineManager(PipelineManager&) = delete;
	PipelineManager& operator=(PipelineManager&) = delete;
private:
	DirectXCommon* dxCommon_ = nullptr;
	//パイプラインデータ
	std::map<std::string,std::unique_ptr<PipelineData>> pipelineDates_;
	std::string nowStateName_;
	//Shaderファイルパス
	std::wstring shaderFilePath_ = L"resources/shaders/";
	std::wstring vsFilePath_ = L".VS.hlsl";
	std::wstring psFilePath_ = L".PS.hlsl";
public:
	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
