#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include "WinApp.h"
#include <dxcapi.h>

namespace {
	//RTVのかず
	const uint32_t kRTVHandleNum = 2;
}

class DirectXCommon{
public://メンバ関数
	//初期化
	void Initialize(WinApp* winApp);
	//描画前処理
	void PreDraw();
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
	//指定の番号のCPUデスクリプタハンドルを取得
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);
	//SRVの指定番号のデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);
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
	//IMGuiの初期化
	void CreateImGui();
private:
	//WindowsAPI
	WinApp* winApp_ = nullptr;

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
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;
	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//RTV
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE,kRTVHandleNum> rtvHandles;
	//スワップチェインリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
	//スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;
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
};

