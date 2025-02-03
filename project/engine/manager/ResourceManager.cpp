#include "ResourceManager.h"
#include <cassert>

const uint32_t ResourceManager::kMaxResourceCount = 512;

void ResourceManager::Initialize(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;

    //デスクリプタヒープの生成。Shader内で触るものなので、ShaderVisibleはture
    descriptorHeap = dxCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxResourceCount, true);
    //デスクリプタ1個分のサイズを取得して記録
    descriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

uint32_t ResourceManager::Allocate() {
    assert(useIndex < kMaxResourceCount);

    //returnする番号を一旦記録しておく
    int index = useIndex;
    //次回のために番号を1進める
    useIndex++;
    //上で記録した番号をreturn
    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE ResourceManager::GetCPUDescriptorHandle(uint32_t index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetGPUDescriptorHandle(uint32_t index) {
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

void ResourceManager::CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels) {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D texture
    srvDesc.Texture2D.MipLevels = MipLevels;

    dxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void ResourceManager::CreateUAVForTexture2D(uint32_t uavIndex, ID3D12Resource* pResource, DXGI_FORMAT Format) {
    assert(pResource != nullptr); // Ensure the resource is valid

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = ConvertToUAVCompatibleFormat(Format);
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D; // 2D texture
    uavDesc.Texture2D.MipSlice = 0;

    dxCommon_->GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &uavDesc, GetCPUDescriptorHandle(uavIndex));
}

void ResourceManager::CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride){
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

void ResourceManager::CreateUAVForStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride) {
    assert(pResource != nullptr); // リソースが有効か確認
    // UAV記述子を初期化
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// 構造化バッファはフォーマット指定なし
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;// 最初の要素
    uavDesc.Buffer.NumElements = numElements;// 構造体の要素数
    uavDesc.Buffer.StructureByteStride = structureByteStride;// 各要素のバイトサイズ
    uavDesc.Buffer.CounterOffsetInBytes = 0;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;// デフォルト

    // UAV 作成
    dxCommon_->GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &uavDesc, GetCPUDescriptorHandle(uavIndex));
}
void ResourceManager::CreateUAVForStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, ID3D12Resource* pCounter, UINT numElements, UINT structureByteStride) {
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

    dxCommon_->GetDevice()->CreateUnorderedAccessView(pResource, pCounter, &uavDesc, GetCPUDescriptorHandle(uavIndex));
}

void ResourceManager::PreDraw() {
    //描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.Get() };
    //デスクリプタヒープを指定する
    dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void ResourceManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t index, bool isSRV) {
    auto commandList = dxCommon_->GetCommandList();
    assert(commandList != nullptr);

    if (isSRV) {
        // SRV descriptor
        auto gpuHandle = GetGPUDescriptorHandle(index);
        assert(gpuHandle.ptr != 0);
        commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, gpuHandle);
    }
    else {
        // UAV descriptor
        auto gpuHandle = GetGPUDescriptorHandle(index);
        assert(gpuHandle.ptr != 0);
        commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, gpuHandle);
    }
}

bool ResourceManager::AvailabilityCheck() {
    return useIndex < kMaxResourceCount;
}

DXGI_FORMAT ResourceManager::ConvertToUAVCompatibleFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM;  // SRGB なしに変換
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM;  // SRGB なしに変換
    default:
        return format; // 変更不要
    }
}
