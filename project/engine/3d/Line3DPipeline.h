#pragma once
#include "BasePipeline.h"

class Line3DPipeline : public BasePipeline {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	std::vector<D3D12_DESCRIPTOR_RANGE> DescriptorRanges();
	std::vector<D3D12_ROOT_PARAMETER> RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges);
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc();
private:

};

