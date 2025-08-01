#pragma once
#include "BasePipeline.h"

class ModelComputePipeline : public BasePipeline{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	//コンピュートシェーダーのデータを取得
	std::vector<D3D12_DESCRIPTOR_RANGE> ComputeDescriptorRanges()override;
	std::vector<D3D12_ROOT_PARAMETER> ComputeRootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges)override;
private:

};

