#include "ModelPipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

std::vector<D3D12_DESCRIPTOR_RANGE> ModelPipeline::DescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(3);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);//テクスチャ
	descriptorRanges[1] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//DirectionalLight
	descriptorRanges[2] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);//PointLight
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> ModelPipeline::RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges) {
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(7);
	rootParameters[0] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//マテリアル
	rootParameters[1] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);//wvp
	rootParameters[2] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[0], 1);//テクスチャ
	rootParameters[3] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);//DirectionalLight
	rootParameters[4] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 2);//camera
	rootParameters[5] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[1], 1);//PointLight
	rootParameters[6] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[2], 1);//SpotLight
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
