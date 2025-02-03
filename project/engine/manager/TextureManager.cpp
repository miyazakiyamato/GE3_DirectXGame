#include "TextureManager.h"
#include "DirectXCommon.h"
#include "ResourceManager.h"

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance(){
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon, ResourceManager* resourceManager){
	dxCommon_ = dxCommon;
	resourceManager_ = resourceManager;
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
	assert(resourceManager_->AvailabilityCheck());

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCommon_->LoadTexture(filePath);

	//追加したテクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	//テクスチャデータの書き込み
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	textureData.intermediateResource = dxCommon_->UploadTextureData(textureData.resource.Get(), mipImages);
	textureData.index = resourceManager_->Allocate();
	textureData.handleCPU = resourceManager_->GetCPUDescriptorHandle(textureData.index);
	textureData.handleGPU = resourceManager_->GetGPUDescriptorHandle(textureData.index);
	textureData.isSRV = true;

	resourceManager_->CreateSRVForTexture2D(textureData.index, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));
}
void TextureManager::LoadRWTexture(const std::string& filePath ,const std::string& objectName) {
	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath + objectName)) {
		//読み込み済みなら早期return
		return;
	}

	//テクスチャ枚数上限チェック
	assert(resourceManager_->AvailabilityCheck());

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCommon_->LoadTexture(filePath);

	//追加したテクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath + objectName];

	//テクスチャデータの書き込み
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateRWTextureResource(textureData.metadata);
	textureData.intermediateResource = dxCommon_->UploadRWTextureData(textureData.resource.Get(), mipImages);
	textureData.index = resourceManager_->Allocate();
	textureData.handleCPU = resourceManager_->GetCPUDescriptorHandle(textureData.index);
	textureData.handleGPU = resourceManager_->GetGPUDescriptorHandle(textureData.index);
	textureData.isSRV = false;

	resourceManager_->CreateUAVForTexture2D(textureData.index, textureData.resource.Get(), textureData.metadata.format);
}
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetHandleGPU(const std::string& filePath){
	//テクスチャ枚数上限チェック
	assert(resourceManager_->AvailabilityCheck());

	TextureData& textureData = textureDatas[filePath];
	return textureData.handleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath){
	//範囲外指定違反チェック
	assert(resourceManager_->AvailabilityCheck());

	if (textureDatas.contains(filePath)) {
		return textureDatas[filePath].metadata;
	}

	assert(false && "テクスチャが見つかりません");
	return textureDatas.begin()->second.metadata;
}

void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const std::string& filePath){
	//テクスチャ枚数上限チェック
	assert(resourceManager_->AvailabilityCheck());

	TextureData& textureData = textureDatas[filePath];

	resourceManager_->SetGraphicsRootDescriptorTable(rootParameterIndex, textureData.index, textureData.isSRV);
}

uint32_t TextureManager::GetIndex(const std::string& filePath){
	//範囲外指定違反チェック
	assert(resourceManager_->AvailabilityCheck());

	TextureData& textureData = textureDatas[filePath];
	return textureData.index;
}