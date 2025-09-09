#include "OffScreenPipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

std::vector<D3D12_STATIC_SAMPLER_DESC> OffScreenPipeline::StaticSamplers(StaticSamplersMode statocSamplersMode){
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
	staticSamplers.resize(2);
	staticSamplers[0] = CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, statocSamplersMode,0);
	staticSamplers[1] = CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_POINT, statocSamplersMode,1);
	return staticSamplers;
}

std::vector<D3D12_DESCRIPTOR_RANGE> OffScreenPipeline::DescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(2);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);//テクスチャ
	descriptorRanges[1] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//深度テクスチャ
	return descriptorRanges;
}
std::vector<D3D12_ROOT_PARAMETER> OffScreenPipeline::RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges){
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(3);
	rootParameters[0] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[0], 1);//テクスチャ
	rootParameters[1] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//マテリアル
	rootParameters[2] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[1], 1);//深度テクスチャ
	return rootParameters;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> OffScreenPipeline::InputElementDesc(){
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};
	return inputElementDescs;
}
