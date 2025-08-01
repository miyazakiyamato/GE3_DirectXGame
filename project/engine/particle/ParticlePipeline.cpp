#include "ParticlePipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

std::vector<D3D12_STATIC_SAMPLER_DESC> ParticlePipeline::StaticSamplers(StaticSamplersMode statocSamplersMode) {
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
	staticSamplers.resize(1);
	staticSamplers[0] = CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, statocSamplersMode,0);
	return staticSamplers;
}

std::vector<D3D12_DESCRIPTOR_RANGE> ParticlePipeline::DescriptorRanges() {
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(1);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);//テクスチャ
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> ParticlePipeline::RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges) {
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(3);
	rootParameters[0] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_VERTEX, descriptorRanges[0],1);//Particle
	rootParameters[1] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[0], 1);//テクスチャ
	rootParameters[2] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);//ビュー投影行列
	return rootParameters;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> ParticlePipeline::InputElementDesc() {
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};
	inputElementDescs.resize(2);
	inputElementDescs[0] = CreateInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT);
	inputElementDescs[1] = CreateInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT);
	return inputElementDescs;
}
