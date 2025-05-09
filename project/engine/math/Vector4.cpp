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