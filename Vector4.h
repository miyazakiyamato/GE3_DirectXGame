#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
/// <summary>
/// 4次元ベクトル
/// </summary>
class Vector4{
public:
	//メンバ変数
	float x;
	float y;
	float z;
	float w;
	//演算子のオーバーロード
};
struct DirectionalLight {
	Vector4 color;//!<ライトの色
	Vector3 direction; //!< ライトの向き
	float intensity;//!< 輝度
};