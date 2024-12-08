#include "Vector2.h"
#include <math.h>

float Vector2::Length()
{
	return Length((*this));
}

Vector2 Vector2::Normalize()
{
	return Normalize((*this));
}

Vector2 Vector2::Abs()
{
	return Abs((*this));
}

Vector2 Vector2::Clamp01()
{
	return Clamp((*this), { 0.0f,0.0f }, { 1.0f,1.0f });
}

Vector2 Vector2::Clamp_11()
{
	return Clamp((*this), { -1.0f,-1.0f }, { 1.0f,1.0f });
}

/// 2次元ベクトルの内積を返す関数
float Vector2::Dot(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

/// 2次元ベクトルのクロス積(外積)を返す関数
float Vector2::Cross(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

/// 2次元ベクトルの長さ(ノルム)を返す関数
float Vector2::Length(float x, float y)
{
	return sqrtf(x * x + y * y);
}

float Vector2::Length(Vector2 v)
{
	return Length(v.x, v.y);
}

/// 2次元ベクトルの正規化した値を返す関数
Vector2 Vector2::Normalize(float x, float y)
{
	float length = Length(x, y);
	if (length != 0) {
		x = x / length;
		y = y / length;
	}
	return Vector2{ x,y };
}

Vector2 Vector2::Normalize(Vector2 v)
{
	return Normalize(v.x, v.y);
}

Vector2 Vector2::Abs(const Vector2& v)
{
	Vector2 result;
	result.x = sqrtf(v.x * v.x);
	result.y = sqrtf(v.y * v.y);
	return result;
}

Vector2 Vector2::Min(const Vector2& v, const Vector2& v2)
{
	Vector2 result;
	result.x = v.x < v2.x ? v.x : v2.x;
	result.y = v.y < v2.y ? v.y : v2.y;
	return result;
}

Vector2 Vector2::Max(const Vector2& v, const Vector2& v2)
{
	Vector2 result;
	result.x = v.x > v2.x ? v.x : v2.x;
	result.y = v.y > v2.y ? v.y : v2.y;
	return result;
}

Vector2 Vector2::Clamp(const Vector2& v, const Vector2& min, const Vector2& max)
{
	Vector2 result = v;
	result.x = result.x < min.x ? min.x : result.x;
	result.y = result.y < min.y ? min.y : result.y;
	result.x = result.x > max.x ? max.x : result.x;
	result.y = result.y > max.y ? max.y : result.y;
	return result;
}

Vector2 Vector2::operator-()
{
	return Vector2(-(*this).x,-(*this).y);
}

Vector2 Vector2::operator+(const Vector2& v2)
{
	Vector2 result;
	result.x = (*this).x + v2.x;
	result.y = (*this).y + v2.y;
	return result;
}

Vector2 Vector2::operator-(const Vector2& v2)
{
	Vector2 result;
	result.x = (*this).x - v2.x;
	result.y = (*this).y - v2.y;
	return result;
}

Vector2 Vector2::operator*(const float& v2)
{
	Vector2 result;
	result.x = (*this).x * v2;
	result.y = (*this).y * v2;
	return result;
}

Vector2 Vector2::operator/(const float& v2)
{
	Vector2 result;
	result.x = (*this).x / v2;
	result.y = (*this).y / v2;
	return result;
}

Vector2& Vector2::operator+=(const Vector2& v2)
{
	(*this).x = (*this).x + v2.x;
	(*this).y = (*this).y + v2.y;

	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v2)
{
	(*this).x = (*this).x - v2.x;
	(*this).y = (*this).y - v2.y;

	return *this;
}

Vector2& Vector2::operator*=(const float& v2)
{
	(*this).x = (*this).x * v2;
	(*this).y = (*this).y * v2;

	return *this;
}

Vector2& Vector2::operator/=(const float& v2)
{
	(*this).x = (*this).x / v2;
	(*this).y = (*this).y / v2;

	return *this;
}
