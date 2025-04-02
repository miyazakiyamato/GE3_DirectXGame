#pragma once
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
	// 誤差を許容する比較関数
	static bool IsEqualWithEpsilon(const Vector4& a, const Vector4& b, float epsilon = 1e-5f);
	//演算子のオーバーロード
};