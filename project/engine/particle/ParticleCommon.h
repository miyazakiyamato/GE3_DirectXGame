#pragma once
#include "DirectXCommon.h"
#include "Camera.h"

class ParticleCommon {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//ブレンドモード
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
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();
private:
	//ブレンドモード設定
	D3D12_BLEND_DESC SetBlendModeNone();
	D3D12_BLEND_DESC SetBlendModeNormal();
	D3D12_BLEND_DESC SetBlendModeAdd();
	D3D12_BLEND_DESC SetBlendModeSubtract();
	D3D12_BLEND_DESC SetBlendModeMultiply();
	D3D12_BLEND_DESC SetBlendModeScreen();
	//ルートシグネチャの作成
	void CreateRootSignature();
private:
	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kAdd;
	static D3D12_BLEND_DESC(ParticleCommon::* spFuncTable[])();
public:
	DirectXCommon* GetDxCommon() { return dxCommon_; }
	BlendMode GetBlendMode() const { return blendMode_; }
	void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }
};
