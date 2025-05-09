#pragma once
#include "Matrix4x4.h"

class Quaternion{
public:
	float x, y, z, w;
	static Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
	static Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);
	static Quaternion Identity();
	static Quaternion Conjugate(const Quaternion& quaternion);
	static Quaternion Inverse(const Quaternion& quaternion);
	static Quaternion Normalize(const Quaternion& quaternion);
	static Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
	static float Norm(const Quaternion& quaternion);
	static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
	static Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	static Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
	static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
	Quaternion operator+(const Quaternion& q);
	Quaternion operator*(const float& f);
	Quaternion operator/(const float& f);
	Quaternion operator-();
};

