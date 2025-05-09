#pragma once
#include "BasePipeline.h"

class ParticlePipeline : public BasePipeline {
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

};