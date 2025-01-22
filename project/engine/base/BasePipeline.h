#pragma once
#include "DirectXCommon.h"
#include "BlendMode.h"
#include <map>

class BasePipeline{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	//初期化
	virtual void Initialize(DirectXCommon* dxCommon) = 0;
	//描画設定
	virtual void DrawSetting(BlendMode blendMode) = 0;
	//ルートシグネチャの作成
	virtual void CreateRootSignature() = 0;
	//グラフィックスパイプラインの生成
	virtual void CreateGraphicsPipeline(BlendMode blendMode) = 0;
protected:
	DirectXCommon* dxCommon_ = nullptr;

	//ブレンドモード
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	std::map<BlendMode, ComPtr<ID3D12PipelineState>> graphicsPipelineState_;
public:
	DirectXCommon* GetDxCommon() { return dxCommon_; }
};

