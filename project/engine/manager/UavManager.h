#pragma once
#include "DirectXCommon.h"

class UavManager{
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	uint32_t ALLocate();
	//UAVの指定番号のデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	//UAV生成(テクスチャ用)
	void CreateUAVForTexture2D(uint32_t uavIndex, ID3D12Resource* pResource, DXGI_FORMAT Format);
	//UAV生成(Structured Buffer用)
	void CreateUAVForStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
	//Counter付きUAV生成(Structured Buffer用)
	void CreateUAVForStructuredBuffer(ID3D12Resource* pResource, ID3D12Resource* pCounter, D3D12_CPU_DESCRIPTOR_HANDLE uavHandle, UINT numElements, UINT structureByteStride);

	void PreDraw();

	void SetGraphicsRootDescriptorTable(UINT RootParaneterIndex, uint32_t uavIndex);

	bool AvailabilityCheck();

	////ImGui用
	//ID3D12DescriptorHeap* GetDescriptorHeapForImGui();
private:
	DirectXCommon* dxCommon_ = nullptr;

	//最大UAV数
	static const uint32_t kMaxUAVCount;
	//UAV用のデスクリプタサイズ
	uint32_t descriptorSize;
	//UAV用でスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	//次に使用するSRVインデックス
	uint32_t useIndex = 1;
};

