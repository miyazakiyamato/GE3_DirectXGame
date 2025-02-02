#include "UavManager.h"
#include <cassert>

const uint32_t UavManager::kMaxUAVCount = 512;

void UavManager::Initialize(DirectXCommon* dxCommon){
	dxCommon_ = dxCommon;

	//デスクリプタヒープの生成。UAVはShader内で触るものなので、ShaderVisibleはture
	descriptorHeap = dxCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxUAVCount, true);
	//デスクリプタ1個分のサイズを取得して記録
	descriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

uint32_t UavManager::ALLocate(){
	assert(useIndex < kMaxUAVCount);

	//returnする番号を一旦記録しておく
	int index = useIndex;
	//次回のために番号を1進める
	useIndex++;
	//上で記録した番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE UavManager::GetCPUDescriptorHandle(uint32_t index){
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE UavManager::GetGPUDescriptorHandle(uint32_t index){
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void UavManager::CreateUAVForTexture2D(uint32_t uavIndex, ID3D12Resource* pResource, DXGI_FORMAT Format){
	assert(pResource != nullptr);// リソースが有効か確認
	
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = Format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    uavDesc.Texture2D.MipSlice = 0;
	// UAV 作成
	dxCommon_->GetDevice()->CreateUnorderedAccessView(pResource, nullptr,&uavDesc, GetCPUDescriptorHandle(uavIndex));
}

void UavManager::CreateUAVForStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride){
	assert(pResource != nullptr); // リソースが有効か確認
	// UAV記述子を初期化
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;// 構造化バッファはフォーマット指定なし
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;// 最初の要素
	uavDesc.Buffer.NumElements = numElements;// 構造体の要素数
	uavDesc.Buffer.StructureByteStride = structureByteStride;// 各要素のバイトサイズ
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;// デフォルト

	// UAV 作成
	dxCommon_->GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &uavDesc, GetCPUDescriptorHandle(uavIndex));
}
void UavManager::CreateUAVForStructuredBuffer(ID3D12Resource* pResource,ID3D12Resource* pCounter, D3D12_CPU_DESCRIPTOR_HANDLE uavHandle,UINT numElements, UINT structureByteStride) {
	assert(pResource != nullptr); // リソースが有効か確認
	assert(pCounter != nullptr);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.StructureByteStride = structureByteStride;
	uavDesc.Buffer.CounterOffsetInBytes = 0; // カウンターリソースの先頭をカウンターとして使う
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	dxCommon_->GetDevice()->CreateUnorderedAccessView(pResource, pCounter, &uavDesc, uavHandle);
}
void UavManager::PreDraw(){
	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.Get() };
	//UAV用のデスクリプタヒープを指定する
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void UavManager::SetGraphicsRootDescriptorTable(UINT RootParaneterIndex, uint32_t uavIndex){
	// コマンドリストが有効か確認
	auto commandList = dxCommon_->GetCommandList();
	assert(commandList != nullptr);

	// UAVインデックスが範囲内か確認
	assert(uavIndex < kMaxUAVCount);

	// GPUデスクリプタハンドルを取得
	auto gpuHandle = GetGPUDescriptorHandle(uavIndex);
	assert(gpuHandle.ptr != 0); // GPUハンドルが有効か確認
	// ルートディスクリプタテーブルを設定
	commandList->SetGraphicsRootDescriptorTable(RootParaneterIndex, gpuHandle);
}

bool UavManager::AvailabilityCheck(){
	if (useIndex < kMaxUAVCount) {
		return true;
	}

	return false;
}
