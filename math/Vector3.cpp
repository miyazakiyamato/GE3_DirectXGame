#include "Vector3.h"
#include "math.h"

float Vector3::Length()
{
	return Length((*this));
}

Vector3 Vector3::Normalize()
{
	return Normalize((*this));
}

Vector3 Vector3::Abs()
{
	return Abs((*this));
}

Vector3 Vector3::Clamp01()
{
	return Clamp((*this),{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f});
}

Vector3 Vector3::Clamp_11()
{
	return Clamp((*this),{-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f});
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector3::Length(const Vector3& v)
{
	return sqrtf(Dot(v, v));
}

Vector3 Vector3::Normalize(const Vector3& v)
{
	Vector3 result = v;
	result = result / Length(result);
	return result;
}

Vector3 Vector3::Abs(const Vector3& v)
{
	Vector3 result;
	result.x = sqrtf(v.x * v.x);
	result.y = sqrtf(v.y * v.y);
	result.z = sqrtf(v.z * v.z);
	return result;
}

Vector3 Vector3::Min(const Vector3& v, const Vector3& v2)
{
	Vector3 result;
	result.x = v.x < v2.x ? v.x : v2.x;
	result.y = v.y < v2.y ? v.y : v2.y;
	result.z = v.z < v2.z ? v.z : v2.z;
	return result;
}

Vector3 Vector3::Max(const Vector3& v, const Vector3& v2)
{
	Vector3 result = v;
	result.x = v.x > v2.x ? v.x : v2.x;
	result.y = v.y > v2.y ? v.y : v2.y;
	result.z = v.z > v2.z ? v.z : v2.z;
	return result;
}

Vector3 Vector3::Clamp(const Vector3& v, const Vector3& min, const Vector3& max)
{
	Vector3 result = v;
	result.x = result.x < min.x ? min.x : result.x;
	result.y = result.y < min.y ? min.y : result.y;
	result.z = result.z < min.z ? min.z : result.z;
	result.x = result.x > max.x ? max.x : result.x;
	result.y = result.y > max.y ? max.y : result.y;
	result.z = result.z > max.z ? max.z : result.z;
	return result;
}

Vector3 Vector3::operator+(const Vector3& v2)
{
	Vector3 result;
	result.x = (*this).x + v2.x;
	result.y = (*this).y + v2.y;
	result.z = (*this).z + v2.z;
	return result;
}

Vector3 Vector3::operator-(const Vector3& v2)
{
	Vector3 result;
	result.x = (*this).x - v2.x;
	result.y = (*this).y - v2.y;
	result.z = (*this).z - v2.z;
	return result;
}

Vector3 Vector3::operator*(const float& v2)
{
	Vector3 result;
	result.x = (*this).x * v2;
	result.y = (*this).y * v2;
	result.z = (*this).z * v2;
	return result;
}

Vector3 Vector3::operator/(const float& v2)
{
	Vector3 result;
	result.x = (*this).x / v2;
	result.y = (*this).y / v2;
	result.z = (*this).z / v2;
	return result;
}
