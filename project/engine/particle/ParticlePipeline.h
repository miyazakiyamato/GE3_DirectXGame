#pragma once
#include "BasePipeline.h"

class ParticlePipeline : public BasePipeline {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	////初期化
	//void Initialize(DirectXCommon* dxCommon);
	////描画設定
	//void DrawSetting(BlendMode blendMode);
	////ルートシグネチャの作成
	//void CreateRootSignature();
	////グラフィックスパイプラインの生成
	//void CreateGraphicsPipeline(BlendMode blendMode);
private:
	std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers(StaticSamplersMode statocSamplersMode);
	std::vector<D3D12_DESCRIPTOR_RANGE> DescriptorRanges();
	std::vector<D3D12_ROOT_PARAMETER> RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges);
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc();
};