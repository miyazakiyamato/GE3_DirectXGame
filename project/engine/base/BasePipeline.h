#pragma once
#include "DirectXCommon.h"
#include "BlendMode.h"
#include <map>

class BasePipeline{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	//ルートシグネチャのデータを取得
	virtual std::vector<D3D12_DESCRIPTOR_RANGE> DescriptorRanges() = 0;
	virtual std::vector<D3D12_ROOT_PARAMETER> RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges) = 0;
	//グラフィックスパイプラインのデータを取得
	virtual std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc() = 0;
	//値をセットしたものを取得
	D3D12_DESCRIPTOR_RANGE CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT baseShaderRegister, UINT numDescriptors);
	D3D12_ROOT_PARAMETER CreateRootParameterCBV(D3D12_SHADER_VISIBILITY visibility, UINT shaderRegister);
	D3D12_ROOT_PARAMETER CreateRootParameterTable(D3D12_SHADER_VISIBILITY visibility, const D3D12_DESCRIPTOR_RANGE& descriptorRegister, UINT descriptorCount);
	D3D12_INPUT_ELEMENT_DESC CreateInputElementDesc(const char* semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot, UINT alignedByteOffset);
};

