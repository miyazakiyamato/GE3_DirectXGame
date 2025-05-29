#include "SpritePipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

std::vector<D3D12_DESCRIPTOR_RANGE> SpritePipeline::DescriptorRanges(){
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges = {};
	descriptorRanges.resize(1);
	descriptorRanges[0] = CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);//テクスチャ
	return descriptorRanges;
}

std::vector<D3D12_ROOT_PARAMETER> SpritePipeline::RootParameters(const std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges){
	std::vector<D3D12_ROOT_PARAMETER> rootParameters = {};
	rootParameters.resize(4);
	rootParameters[0] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//マテリアル
	rootParameters[1] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);//座標
	rootParameters[2] = CreateRootParameterTable(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRanges[0], 1);//テクスチャ
	rootParameters[3] = CreateRootParameterCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);//ライト
	return rootParameters;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> SpritePipeline::InputElementDesc(){
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};
	inputElementDescs.resize(3);
	inputElementDescs[0] = CreateInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT);
	inputElementDescs[1] = CreateInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT);
	inputElementDescs[2] = CreateInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT);
	return inputElementDescs;
}