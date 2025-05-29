#pragma once
#include "BasePipeline.h"

class SpritePipeline : public BasePipeline {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	//ルートシグネチャのデータを取得
	std::vector<D3D12_DESCRIPTOR_RANGE> DescriptorRanges()override;
	std::vector<D3D12_ROOT_PARAMETER> RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges)override;
	//グラフィックスパイプラインのデータを取得
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc()override;
private:

};