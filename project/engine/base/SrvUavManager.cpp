#include "SrvUavManager.h"
#include <cassert>
#include "ParticleManager.h"

const uint32_t SrvUavManager::kMaxSRVCount = 512;

void SrvUavManager::Initialize(DirectXCommon* dxCommon){
	dxCommon_ = dxCommon;

	//デスクリプタヒープの生成。SRVはShader内で触るものなので、ShaderVisibleはture
	descriptorHeap_ = dxCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
	//デスクリプタ1個分のサイズを取得して記録
	descriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

uint32_t SrvUavManager::Allocate(){
	assert(useIndex < kMaxSRVCount);
	
	//returnする番号を一旦記録しておく
	int index = useIndex;
	//次回のために番号を1進める
	useIndex++;
	//上で記録した番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvUavManager::GetCPUDescriptorHandle(uint32_t index){
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvUavManager::GetGPUDescriptorHandle(uint32_t index){
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void SrvUavManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, DirectX::TexMetadata metadata){
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE; //キューブマップテクスチャ
		srvDesc.TextureCube.MostDetailedMip = 0; //最も詳細なミップレベル
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f; //最小LODクランプ値
	} else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}

	dxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvUavManager::CreateSRVforDepthTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format){
	assert(pResource != nullptr); // リソースが有効か確認
	// SRV記述子を初期化
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format; // 深度テクスチャのフォーマット
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MostDetailedMip = 0; // 最も詳細なミップレベル
	srvDesc.Texture2D.MipLevels = 1; // ミップレベル数
	srvDesc.Texture2D.PlaneSlice = 0; // プレーンスライス（通常は0）
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f; // 最小LODクランプ値
	// SRV作成
	dxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvUavManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride) {
	assert(pResource != nullptr); // リソースが有効か確認
	// SRV記述子を初期化
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 構造化バッファはフォーマット指定なし
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0; // 最初の要素
	srvDesc.Buffer.NumElements = numElements; // 構造体の要素数
	srvDesc.Buffer.StructureByteStride = structureByteStride; // 各要素のバイトサイズ
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE; // デフォルト
	// SRV作成
	dxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvUavManager::CreateUAVforStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByStride){
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; // 構造化バッファはフォーマット指定なし
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0; // 最初の要素
	uavDesc.Buffer.NumElements = numElements; // 構造体の要素数
	uavDesc.Buffer.CounterOffsetInBytes = 0; // カウンターオフセット（通常は0）
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE; // デフォルト
	uavDesc.Buffer.StructureByteStride = structureByStride; // 各要素のバイトサイズ
	dxCommon_->GetDevice()->CreateUnorderedAccessView(
		pResource,
		nullptr, // カウンターリソースは使用しない
		&uavDesc,
		GetCPUDescriptorHandle(uavIndex) // UAVのCPUハンドルを取得
	);
}

void SrvUavManager::PreDraw(){
	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	//SRV用のデスクリプタヒープを指定する
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void SrvUavManager::SetGraphicsRootDescriptorTable(UINT RootParaneterIndex, uint32_t srvIndex){
	// コマンドリストが有効か確認
	auto commandList = dxCommon_->GetCommandList();
	assert(commandList != nullptr);

	// SRVインデックスが範囲内か確認
	assert(srvIndex < kMaxSRVCount);

	// GPUデスクリプタハンドルを取得
	auto gpuHandle = GetGPUDescriptorHandle(srvIndex);
	assert(gpuHandle.ptr != 0); // GPUハンドルが有効か確認
	// ルートディスクリプタテーブルを設定
	commandList->SetGraphicsRootDescriptorTable(RootParaneterIndex, gpuHandle);
}

void SrvUavManager::SetComputeRootDescriptorTable(UINT RootParaneterIndex, uint32_t srvIndex){
	// コマンドリストが有効か確認
	auto commandList = dxCommon_->GetCommandList();
	assert(commandList != nullptr);

	// SRVインデックスが範囲内か確認
	assert(srvIndex < kMaxSRVCount);

	// GPUデスクリプタハンドルを取得
	auto gpuHandle = GetGPUDescriptorHandle(srvIndex);
	assert(gpuHandle.ptr != 0); // GPUハンドルが有効か確認
	// ルートディスクリプタテーブルを設定
	commandList->SetComputeRootDescriptorTable(RootParaneterIndex, gpuHandle);
}

bool SrvUavManager::AvailabilityCheck(){
	if (useIndex < kMaxSRVCount) {
		return true;
	}

	return false;
}

ID3D12DescriptorHeap* SrvUavManager::GetDescriptorHeapForImGui()
{
	return descriptorHeap_.Get();
}

