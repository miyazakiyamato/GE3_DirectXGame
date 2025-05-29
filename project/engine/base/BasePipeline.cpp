#include "BasePipeline.h"

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