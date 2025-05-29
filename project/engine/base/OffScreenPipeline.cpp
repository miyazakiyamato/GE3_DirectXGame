#include "OffScreenPipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

std::vector<D3D12_DESCRIPTOR_RANGE> OffScreenPipeline::DescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(1);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);//テクスチャ
	return descriptorRanges;
}
std::vector<D3D12_ROOT_PARAMETER> OffScreenPipeline::RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges){
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(1);
	rootParameters[0] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[0], 1);//テクスチャ
	return rootParameters;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> OffScreenPipeline::InputElementDesc(){
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};
	return inputElementDescs;
}
