#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>
#include "DirectXTex.h"

class DirectXCommon;
class ResourceManager;
class TextureManager{
private://構造体
	//テクスチャ一枚分のデータ
	struct TextureData{
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		uint32_t index;
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU;
		bool isSRV = true;
	};
public://メンバ関数
	//シングルインスタンスの取得
	static TextureManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon, ResourceManager* resourceManager);
	//終了
	void Finalize();
	//テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);
	void LoadRWTexture(const std::string& filePath, const std::string& objectName);
	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(const std::string& filePath);
	//メタデータの取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
	//
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const std::string& filePath);
	uint32_t GetIndex(const std::string& filePath);
private://シングルインスタンス
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;
private://メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	ResourceManager* resourceManager_ = nullptr;
	//テクスチャデータ
	std::unordered_map<std::string,TextureData> textureDatas;
public:

};

