#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>
#include "DirectXTex.h"

class DirectXCommon;
class SrvManager;
class UavManager;
class TextureManager{
private://構造体
	//テクスチャ一枚分のデータ
	struct TextureData{
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};
	//UAV 用
	struct RWTextureData {
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		uint32_t uavIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE uavHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE uavHandleGPU;
	};
public://メンバ関数
	//シングルインスタンスの取得
	static TextureManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, UavManager* uavManager);
	//終了
	void Finalize();
	//テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);
	void LoadRWTexture(const std::string& filePath);

	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavHandleGPU(const std::string& filePath);
	//メタデータの取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
	//SRVインデックスの取得
	uint32_t GetSrvIndex(const std::string& filePath);
	//UAVインデックスの取得
	uint32_t GetUavIndex(const std::string& filePath);
private://シングルインスタンス
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;
private://メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	UavManager* uavManager_ = nullptr;
	//テクスチャデータ
	std::unordered_map<std::string,TextureData> textureDatas;
	std::unordered_map<std::string,RWTextureData> rwTextureDatas;
public:

};

