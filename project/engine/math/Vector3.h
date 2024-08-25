#pragma once
/// <summary>
/// 3次元ベクトル
/// </summary>
class Vector3{
public:
	//メンバ変数
	float x;
	float y;
	float z;
	//メンバ関数
	float Length();
	Vector3 Normalize();
	Vector3 Abs();
	Vector3 Clamp01();
	Vector3 Clamp_11();

	static float Dot(const Vector3& v1, const Vector3& v2);
	static float Length(const Vector3& v);
	static Vector3 Normalize(const Vector3& v);
	static Vector3 Abs(const Vector3& v);
	static Vector3 Min(const Vector3& v, const Vector3& v2);
	static Vector3 Max(const Vector3& v, const Vector3& v2);
	static Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max);
	//演算子のオーバーロード
	Vector3 operator+(const Vector3& v2);
	Vector3 operator-(const Vector3& v2);
	Vector3 operator*(const float& v2);
	Vector3 operator/(const float& v2);
};

