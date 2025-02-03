#pragma once
#include "DirectXCommon.h"

class ResourceManager{
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	uint32_t Allocate();

	//SRVの指定番号のデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	//SRV生成(テクスチャ用)
	void CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	//UAV生成(テクスチャ用)
	void CreateUAVForTexture2D(uint32_t uavIndex, ID3D12Resource* pResource, DXGI_FORMAT Format);
	//SRV生成(Structured Buffer用)
	void CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
	//UAV生成(Structured Buffer用)
	void CreateUAVForStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
	//Counter付きUAV生成(Structured Buffer用)
	void CreateUAVForStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, ID3D12Resource* pCounter, UINT numElements, UINT structureByteStride);

	void PreDraw();

	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t index, bool isSRV = true);

	bool AvailabilityCheck();
	//ForMat変換
	DXGI_FORMAT ConvertToUAVCompatibleFormat(DXGI_FORMAT format);

	//ImGui用
	ID3D12DescriptorHeap* GetDescriptorHeapForImGui() { return descriptorHeap.Get(); }
private:
	DirectXCommon* dxCommon_ = nullptr;

	//最大SRV&UAR数
	static const uint32_t kMaxResourceCount;
	//デスクリプタサイズ
	uint32_t descriptorSize;
	//デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	//次に使用するSRVインデックス
	uint32_t useIndex = 1;
};

