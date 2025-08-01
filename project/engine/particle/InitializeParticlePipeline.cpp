#include "InitializeParticlePipeline.h"

std::vector<D3D12_DESCRIPTOR_RANGE> InitializeParticlePipeline::ComputeDescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(2);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	descriptorRanges[1] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> InitializeParticlePipeline::ComputeRootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges){
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(2);
	rootParameters[0] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[0], 1);
	rootParameters[1] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[1], 1);
	return rootParameters;
}
