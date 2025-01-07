#pragma once
#include "DirectXCommon.h"
#include "BlendMode.h"
#include <map>

class PipelineManager {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//描画設定
	void DrawSetting(BlendMode blendMode);
	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline(BlendMode blendMode);
private:
	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//ブレンドモード
	std::map<BlendMode, ComPtr<ID3D12PipelineState>> graphicsPipelineState_;

	BlendMode nowPipelineState_ = BlendMode::kNormal;
public:
	DirectXCommon* GetDxCommon() { return dxCommon_; }
};
