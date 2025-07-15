#include "Vector4.h"
#include <cmath>

bool Vector4::IsEqualWithEpsilon(const Vector4& a, const Vector4& b, float epsilon) {
	return std::fabs(a.x - b.x) < epsilon &&
		std::fabs(a.y - b.y) < epsilon &&
		std::fabs(a.z - b.z) < epsilon &&
		std::fabs(a.w - b.w) < epsilon;
}

Vector4 Vector4::Random(std::mt19937& randomEngine, const Vector4& vMin, const Vector4& vMax) {
	std::uniform_real_distribution<float> distX(vMin.x, vMax.x);
	std::uniform_real_distribution<float> distY(vMin.y, vMax.y);
	std::uniform_real_distribution<float> distZ(vMin.z, vMax.z);
	std::uniform_real_distribution<float> distW(vMin.w, vMax.w);
	return { distX(randomEngine), distY(randomEngine), distZ(randomEngine) ,distW(randomEngine) };
}

Vector4 Vector4::operator-(){
	return Vector4{ -x, -y, -z, -w };
}

Vector4 Vector4::operator+(const Vector4& v2){
	return { x + v2.x, y + v2.y, z + v2.z , w + v2.w };
}

Vector4 Vector4::operator-(const Vector4& v2){
	return { x - v2.x, y - v2.y, z - v2.z, w - v2.w };
}

Vector4 Vector4::operator*(const float& v2){
	return { x * v2, y * v2, z * v2, w * v2 };
}

Vector4 Vector4::operator/(const float& v2){
	return { x / v2, y / v2, z / v2, w / v2 };
}

Vector4& Vector4::operator+=(const Vector4& v2){
	x += v2.x;
	y += v2.y;
	z += v2.z;
	w += v2.w;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& v2){
	x -= v2.x;
	y -= v2.y;
	z -= v2.z;
	w -= v2.w;
	return *this;
}

Vector4& Vector4::operator*=(const float& v2){
	x *= v2;
	y *= v2;
	z *= v2;
	w *= v2;
	return *this;
}

Vector4& Vector4::operator/=(const float& v2){
	x /= v2;
	y /= v2;
	z /= v2;
	w /= v2;
	return *this;
}
