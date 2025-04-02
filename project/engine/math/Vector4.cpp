#include "Vector4.h"
#include <cmath>

bool Vector4::IsEqualWithEpsilon(const Vector4& a, const Vector4& b, float epsilon) {
	return std::fabs(a.x - b.x) < epsilon &&
		std::fabs(a.y - b.y) < epsilon &&
		std::fabs(a.z - b.z) < epsilon &&
		std::fabs(a.w - b.w) < epsilon;
}