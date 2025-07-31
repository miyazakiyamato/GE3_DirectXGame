#include "ModelPipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

std::vector<D3D12_STATIC_SAMPLER_DESC> ModelPipeline::StaticSamplers(StaticSamplersMode statocSamplersMode) {
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
	staticSamplers.resize(1);
	staticSamplers[0] = CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, statocSamplersMode,0);
	return staticSamplers;
}

std::vector<D3D12_DESCRIPTOR_RANGE> ModelPipeline::DescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(4);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);//テクスチャ
	descriptorRanges[1] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//PointLight
	descriptorRanges[2] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);//SpotLight
	descriptorRanges[3] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1);//環境マップ
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> ModelPipeline::RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges) {
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(8);
	rootParameters[0] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//マテリアル
	rootParameters[1] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);//wvp
	rootParameters[2] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[0], 1);//テクスチャ
	rootParameters[3] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);//DirectionalLight
	rootParameters[4] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 2);//camera
	rootParameters[5] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[1], 1);//PointLight
	rootParameters[6] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[2], 1);//SpotLight
	rootParameters[7] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[3], 1);//環境マップ
	return rootParameters;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> ModelPipeline::InputElementDesc(){
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};
	inputElementDescs.resize(3);
	inputElementDescs[0] = CreateInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT);
	inputElementDescs[1] = CreateInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT);
	inputElementDescs[2] = CreateInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT);
	return inputElementDescs;
}

std::vector<D3D12_DESCRIPTOR_RANGE> ModelPipeline::ComputeDescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(4);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	descriptorRanges[1] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	descriptorRanges[2] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	descriptorRanges[3] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> ModelPipeline::ComputeRootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges){
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(5);
	rootParameters[0] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_ALL, 0);
	rootParameters[1] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[0], 1);
	rootParameters[2] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[1], 1);
	rootParameters[3] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[2], 1);
	rootParameters[4] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[3], 1);
	return rootParameters;
}
