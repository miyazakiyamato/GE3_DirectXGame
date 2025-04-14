#pragma once
#include <random>

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

	static Vector3 Add(const Vector3& v1, const Vector3& v2);
	static Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	static Vector3 Multiply(float scalar, const Vector3& v);
	static Vector3 Divide(float scalar, const Vector3& v);
	static float Dot(const Vector3& v1, const Vector3& v2);
	static float Length(const Vector3& v);
	static Vector3 Normalize(const Vector3& v);
	static Vector3 Abs(const Vector3& v);
	static Vector3 Min(const Vector3& v, const Vector3& v2);
	static Vector3 Max(const Vector3& v, const Vector3& v2);
	static Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max);
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);
	static Vector3 Project(const Vector3& v1, const Vector3& v2);
	static Vector3 Perpendicular(const Vector3& vector);
	static Vector3 Reflect(const Vector3& input, const Vector3& normal);
	/*static Vector3 ClosestPoint(const Vector3& point, const Line& line);
	static Vector3 ClosestPoint(const Vector3& point, const Ray& ray);
	static Vector3 ClosestPoint(const Vector3& point, const Segment& segment);*/
	static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
	static Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float& t);
	static Vector3 Random(std::mt19937& randomEngine, const Vector3& vMin, const Vector3& vMax);
	//演算子のオーバーロード
	Vector3 operator-();
	Vector3 operator+(const Vector3& v2);
	Vector3 operator-(const Vector3& v2);
	Vector3 operator*(const float& v2);
	Vector3 operator/(const float& v2);
	Vector3& operator+=(const Vector3& v2);
	Vector3& operator-=(const Vector3& v2);
	Vector3& operator*=(const float& v2);
	Vector3& operator/=(const float& v2);
};

