#pragma once
#include "DirectXCommon.h"
#include "BlendMode.h"
#include "BasePipeline.h"
#include <map>

enum class PipelineState {
	kModel,
	kParticle,
	kSprite,
};

class PipelineManager {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	//シングルトンインスタンスの取得
	static PipelineManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//終了
	void Finalize();
	//描画設定
	void DrawSetting(PipelineState pipelineState, BlendMode blendMode);
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline(PipelineState pipelineState,BlendMode blendMode);
private://シングルインスタンス
	static PipelineManager* instance;

	PipelineManager() = default;
	~PipelineManager() = default;
	PipelineManager(PipelineManager&) = delete;
	PipelineManager& operator=(PipelineManager&) = delete;
private:
	DirectXCommon* dxCommon_ = nullptr;
	//ブレンドモード
	std::map<PipelineState, std::unique_ptr<BasePipeline>> graphicsPipelineState_;
public:
	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
