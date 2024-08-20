#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"


class DirectXCommon;
class TextureManager{
private:
	//テクスチャ一枚分のデータ
	struct TextureData{
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};
public:
	//シングルインスタンスの取得
	static TextureManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//終了
	void Finalize();
	//テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);

	//SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);
	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);
private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	//
	DirectXCommon* dxCommon_ = nullptr;
	//テクスチャデータ
	std::vector<TextureData> textureDatas;
	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;
};

