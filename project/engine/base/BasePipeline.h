#pragma once
#include "DirectXCommon.h"
#include "BlendMode.h"
#include <map>
enum class StaticSamplersMode {
	kwrap, //!< Wrapモード
	kclamp, //!< Clampモード
};

class BasePipeline{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	virtual std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers(StaticSamplersMode staticSamplersMode);
	//ルートシグネチャのデータを取得
	virtual std::vector<D3D12_DESCRIPTOR_RANGE> DescriptorRanges();
	virtual std::vector<D3D12_ROOT_PARAMETER> RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges);
	//パイプラインのデータを取得
	virtual std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDesc();
	//コンピュートシェーダーのデータを取得
	virtual std::vector<D3D12_DESCRIPTOR_RANGE> ComputeDescriptorRanges();
	virtual std::vector<D3D12_ROOT_PARAMETER> ComputeRootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges);
	//値をセットしたものを取得
	D3D12_STATIC_SAMPLER_DESC CreateStaticSampler(D3D12_FILTER filter, StaticSamplersMode staticSamplersMode, UINT ShaderRegister);
	D3D12_DESCRIPTOR_RANGE CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT baseShaderRegister, UINT numDescriptors);
	D3D12_ROOT_PARAMETER CreateRootParameterCBV(D3D12_SHADER_VISIBILITY visibility, UINT shaderRegister);
	D3D12_ROOT_PARAMETER CreateRootParameterTable(D3D12_SHADER_VISIBILITY visibility, const D3D12_DESCRIPTOR_RANGE& descriptorRegister, UINT descriptorCount);
	D3D12_INPUT_ELEMENT_DESC CreateInputElementDesc(const char* semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot, UINT alignedByteOffset);
};

