#pragma once
#include "BasePipeline.h"

class EmitParticlePipeline : public BasePipeline {
public:
	//コンピュートシェーダーのデータを取得
	std::vector<D3D12_DESCRIPTOR_RANGE> ComputeDescriptorRanges()override;
	std::vector<D3D12_ROOT_PARAMETER> ComputeRootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges)override;
};