#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include <string>
#include <thread>
#include <chrono>
#include "WinApp.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Vector4.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

namespace {
	//RTVのかず
	const uint32_t kRTVHandleSwapChainNum = 2;
	const uint32_t kRTVHandleRenderTextureNum = 1;
	const uint32_t kRTVHandleNum = 3;
}

class SrvManager;
class DirectXCommon{
public://メンバ関数
	//初期化
	void Initialize(WinApp* winApp);
	//描画前処理(レンダーテクスチャ)
	void RenderTexturePreDraw();
	//描画前処理(スワップチェイン)
	void SwapChainPreDraw();
	//offScreen描画
	void OffScreenDraw();
	//描画後処理
	void PostDraw();
	//デバイスの初期化
	void CreateDevice();
	//コマンド関連の初期化
	void CreateCommand();
	//スワップチェインの生成
	void CreateSwapChain();
	//深度バッファの生成
	void CreateDepthStencil();
	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, const bool& shaderVisible);
	//各種でスクリプタヒープの生成
	void CreateDescriptorHeaps();
	//レンダーターゲットビューの初期化
	void CreateRTVDescriptorHeaps();
	void CreateOffScreenSRV(SrvManager* srvManager);
	//指定の番号のCPUデスクリプタハンドルを取得
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);
	//深度ステンシルビューの初期化
	void CreateDepthStencilView();
	//フェンスの初期化
	void CreateFence();
	//ビューポート矩形の初期化
	void CreateViewportRect();
	//シザリング矩形
	void CreateScissorRect();
	//DXCコンパイラの生成
	void CreateDXCCompiler();
	//シェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile);
	//RenderTextureResourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(DXGI_FORMAT format,const Vector4& clearColor);
	//バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const size_t& sizeInbytes);
	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	//テクスチャデータの転送
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	//テクスチャファイルの読み込み
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
private://メンバ関数
	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;
private://メンバ変数
	//WindowsAPI
	WinApp* winApp_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	//コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;
	//デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	//デスクリプタヒープサイズ
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;
	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
	//RTV
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE,kRTVHandleNum> rtvHandles;
	//RenderTextureResource
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource = nullptr;
	D3D12_CLEAR_VALUE clearValue_;
	uint32_t offScreenSRVIndex = 0;
	//スワップチェインリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
	//スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//シザー矩形
	D3D12_RECT scissorRect{};
	//DXCコンパイラ
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	//インクルードハンドラ
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	//フェンス値
	UINT16 fenceVal = 0;
public:
	ID3D12Device* GetDevice() { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }
	//バックバッファの数を取得
	size_t GetBackBufferCount() const { return swapChainDesc.BufferCount; }
	//スワップチェインの取得
	IDXGISwapChain4* GetSwapChain() { return swapChain.Get(); }

	ID3D12DescriptorHeap* GetDsvDescriptorHeap() { return dsvDescriptorHeap.Get(); }
};

