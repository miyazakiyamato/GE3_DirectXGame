#include "Line3DPipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

std::vector<D3D12_DESCRIPTOR_RANGE> Line3DPipeline::DescriptorRanges() {
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> Line3DPipeline::RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges) {
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(1);
	rootParameters[0] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);//座標と色
	return rootParameters;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> Line3DPipeline::InputElementDesc() {
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};
	inputElementDescs.resize(2);
	inputElementDescs[0] = CreateInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT);
	inputElementDescs[1] = CreateInputElementDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT);
	return inputElementDescs;
}