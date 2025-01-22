#include "TextureManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager){
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;
}

void TextureManager::Finalize(){
	delete instance;
	instance = nullptr;
}

void TextureManager::LoadTexture(const std::string& filePath){
	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}

	//テクスチャ枚数上限チェック
	assert(srvManager_->AvailabilityCheck());

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCommon_->LoadTexture(filePath);

	//追加したテクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	//テクスチャデータの書き込み
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	textureData.intermediateResource = dxCommon_->UploadTextureData(textureData.resource.Get(), mipImages);
	textureData.srvIndex = srvManager_->ALLocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	srvManager_->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath){
	//テクスチャ枚数上限チェック
	assert(srvManager_->AvailabilityCheck());

	TextureData& textureData = textureDatas[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath){
	//範囲外指定違反チェック
	assert(srvManager_->AvailabilityCheck());

	TextureData& textureData = textureDatas[filePath];
	return textureData.metadata;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath){
	//範囲外指定違反チェック
	assert(srvManager_->AvailabilityCheck());

	TextureData& textureData = textureDatas[filePath];
	return textureData.srvIndex;
}
