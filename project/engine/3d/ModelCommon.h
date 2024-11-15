#pragma once
#include "DirectXCommon.h"
#include "Camera.h"

class ModelCommon{
public:
	enum class BlendMode {
		kNone,      //!< ブレンドなし
		kNormal,    //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		kAdd,       //!< 加算。Src * SrcA + Dest * 1
		kSubtract,  //!< 減算。Dest * 1 - Src * SrcA
		kMultiply,  //!< 乗算。Src * 0 + Dest * Src
		kScreen,    //!< スクリーン。Src * (1 - Dest) + Dest * 1
		kExclusion, //!< 除外。(1 - Dest) * Src + (1 - Src) * Dest

		kCountOfBlendMode,    //!< ブレンドモード数。利用してはいけない
	};

public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//共通描画設定
	void DrawCommonSetting();
	//ブレンドモード設定
	D3D12_BLEND_DESC SetBlendModeNone();
	D3D12_BLEND_DESC SetBlendModeNormal();
	D3D12_BLEND_DESC SetBlendModeAdd();
	D3D12_BLEND_DESC SetBlendModeSubtract();
	D3D12_BLEND_DESC SetBlendModeMultiply();
	D3D12_BLEND_DESC SetBlendModeScreen();

private:
	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();
private:
	DirectXCommon* dxCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;
	static D3D12_BLEND_DESC (ModelCommon::*spFuncTable[])();
public:
	DirectXCommon* GetDxCommon() { return dxCommon_; }
	void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }
};

