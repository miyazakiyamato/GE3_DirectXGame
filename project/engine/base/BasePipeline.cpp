#include "BasePipeline.h"

D3D12_STATIC_SAMPLER_DESC BasePipeline::CreateStaticSampler(D3D12_FILTER filter,StaticSamplersMode staticSamplersMode, UINT ShaderRegister){
	D3D12_STATIC_SAMPLER_DESC staticSampler = {};
	staticSampler.Filter = filter;//バイナリフィルタ
	switch (staticSamplersMode) {
	default:
	case StaticSamplersMode::kwrap://Wrapモード
		staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
		staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		break;
	case StaticSamplersMode::kclamp://Clampモード
		staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//0~1の範囲外をClamp
		staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		break;
	}
	staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSampler.MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSampler.ShaderRegister = ShaderRegister;//レジスタ番号
	staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	return staticSampler;
}

D3D12_DESCRIPTOR_RANGE BasePipeline::CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT baseShaderRegister, UINT numDescriptors){
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.BaseShaderRegister = baseShaderRegister;//始まり
	descriptorRange.NumDescriptors = numDescriptors;//数
	descriptorRange.RangeType = type;//読み書きタイプ
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算
	return descriptorRange;
}

D3D12_ROOT_PARAMETER BasePipeline::CreateRootParameterCBV(D3D12_SHADER_VISIBILITY visibility, UINT shaderRegister){
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility = visibility;
	rootParameter.Descriptor.ShaderRegister = shaderRegister;
	return rootParameter;
}

D3D12_ROOT_PARAMETER BasePipeline::CreateRootParameterTable(D3D12_SHADER_VISIBILITY visibility, const D3D12_DESCRIPTOR_RANGE& descriptorRegister, UINT descriptorCount){
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter.ShaderVisibility = visibility;
	rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRegister;
	rootParameter.DescriptorTable.NumDescriptorRanges = descriptorCount;
	return rootParameter;
}

D3D12_INPUT_ELEMENT_DESC BasePipeline::CreateInputElementDesc(const char* semanticName, UINT semanticIndex, DXGI_FORMAT format,UINT inputSlot, UINT alignedByteOffset){
	D3D12_INPUT_ELEMENT_DESC inputElementDesc = {};
	inputElementDesc.SemanticName = semanticName;
	inputElementDesc.SemanticIndex = semanticIndex;
	inputElementDesc.Format = format;
	inputElementDesc.InputSlot = inputSlot;
	inputElementDesc.AlignedByteOffset = alignedByteOffset;
	return inputElementDesc;
}
std::vector<D3D12_DESCRIPTOR_RANGE> BasePipeline::ComputeDescriptorRanges(){
	return std::vector<D3D12_DESCRIPTOR_RANGE>();
}

std::vector<D3D12_ROOT_PARAMETER> BasePipeline::ComputeRootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges){
	return std::vector<D3D12_ROOT_PARAMETER>();
}
