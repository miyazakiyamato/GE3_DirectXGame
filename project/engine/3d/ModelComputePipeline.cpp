#include "ModelComputePipeline.h"

std::vector<D3D12_DESCRIPTOR_RANGE> ModelComputePipeline::ComputeDescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(4);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	descriptorRanges[1] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	descriptorRanges[2] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	descriptorRanges[3] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> ModelComputePipeline::ComputeRootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges){
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(5);
	rootParameters[0] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_ALL, 0);
	rootParameters[1] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[0], 1);
	rootParameters[2] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[1], 1);
	rootParameters[3] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[2], 1);
	rootParameters[4] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_ALL, descriptorRanges[3], 1);
	return rootParameters;
}
