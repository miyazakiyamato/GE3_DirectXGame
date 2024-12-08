#pragma once
/// <summary>
/// 2次元ベクトル
/// </summary>
class Vector2{
public:
	//メンバ変数
	float x;
	float y;

	//メンバ関数
	float Length();
	Vector2 Normalize();
	Vector2 Abs();
	Vector2 Clamp01();
	Vector2 Clamp_11();

	static float Dot(const Vector2& v1, const Vector2& v2);
	static float Cross(const Vector2& v1, const Vector2& v2);
	static float Length(float x, float y);
	static float Length(Vector2 v);
	static Vector2 Normalize(float x, float y);
	static Vector2 Normalize(Vector2 v);
	static Vector2 Abs(const Vector2& v);
	static Vector2 Min(const Vector2& v, const Vector2& v2);
	static Vector2 Max(const Vector2& v, const Vector2& v2);
	static Vector2 Clamp(const Vector2& v, const Vector2& min, const Vector2& max);
	//演算子のオーバーロード
	Vector2 operator-();
	Vector2 operator+(const Vector2& v2);
	Vector2 operator-(const Vector2& v2);
	Vector2 operator*(const float& v2);
	Vector2 operator/(const float& v2);
	Vector2& operator+=(const Vector2& v2);
	Vector2& operator-=(const Vector2& v2);
	Vector2& operator*=(const float& v2);
	Vector2& operator/=(const float& v2);
};

