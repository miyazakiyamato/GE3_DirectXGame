#pragma once
#include "BasePipeline.h"

class SkyboxPipeline : public BasePipeline{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	//スタティックサンプラーを取得
	std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers(StaticSamplersMode statocSamplersMode)override;
	//ルートシグネチャのデータを取得
	std::vector<D3D12_DESCRIPTOR_RANGE> DescriptorRanges()override;
	std::vector<D3D12_ROOT_PARAMETER> RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges)override;
	//グラフィックスパイプラインのデータを取得
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc()override;
private:

};

