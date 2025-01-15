#pragma once
#include <d3d12.h>

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

//BlendModeのD3D12_BLEND_DESCを取得する関数
namespace BlendModeHelper {
    D3D12_BLEND_DESC SetBlendModeNone();
    D3D12_BLEND_DESC SetBlendModeNormal();
    D3D12_BLEND_DESC SetBlendModeAdd();
    D3D12_BLEND_DESC SetBlendModeSubtract();
    D3D12_BLEND_DESC SetBlendModeMultiply();
    D3D12_BLEND_DESC SetBlendModeScreen();

    //BlendModeを対応する関数にマッピングする関数テーブル
    typedef D3D12_BLEND_DESC(*BlendModeFunc)();
    extern BlendModeFunc BlendModeFunctionTable[];
}
