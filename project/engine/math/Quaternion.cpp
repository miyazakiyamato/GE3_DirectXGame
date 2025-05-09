#include "Quaternion.h"
#include <math.h>

Matrix4x4 Quaternion::MakeRotateAxisAngle(const Vector3& axis, float angle)
{
	// 回転軸を正規化
	Vector3 axisNormal = axis;
	axisNormal = axisNormal.Normalize();

	//何度も使う計算
	float cosTheta = cosf(angle);
	float sinTheta = -sinf(angle);
	Vector3 axisCos = axisNormal * (1.0f - cosTheta);

	Matrix4x4 rotateMatrix = {
		cosTheta + axisNormal.x * axisCos.x, axisNormal.x * axisCos.y + -axisNormal.z * sinTheta, axisNormal.x * axisCos.z + axisNormal.y * sinTheta, 0.0f,
		axisNormal.x * axisCos.y + axisNormal.z * sinTheta,cosTheta + axisNormal.y * axisCos.y, axisNormal.y * axisCos.z + -axisNormal.x * sinTheta, 0.0f,
		axisNormal.x * axisCos.z + -axisNormal.y * sinTheta, axisNormal.y * axisCos.z + axisNormal.x * sinTheta,cosTheta + axisNormal.z * axisCos.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return rotateMatrix;
}
Matrix4x4 Quaternion::DirectionToDirection(const Vector3& from, const Vector3& to)
{
	// fromとtoの正規化
	Vector3 fromNormal = from;
	fromNormal = fromNormal.Normalize();
	Vector3 toNormal = to;
	toNormal = toNormal.Normalize();

	// 回転軸（クロス積）
	Vector3 axis = Vector3::Cross(fromNormal, toNormal);

	// 回転角度（内積からコサインを計算してアークコサインで角度に変換）
	float angle = acosf(Vector3::Dot(fromNormal, toNormal));
	if (axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f) {
		if (toNormal.x != 0 || toNormal.y != 0) {
			axis = { toNormal.y,-toNormal.x,0 };
		}
		else if (toNormal.x != 0 || toNormal.z != 0) {
			axis = { toNormal.z,0,-toNormal.x };
		}
	}
	// 回転軸を正規化
	axis = axis.Normalize();

	// 回転行列を計算
	Matrix4x4 rotationMatrix = MakeRotateAxisAngle(axis, angle);

	return rotationMatrix;
}

Quaternion Quaternion::Identity()
{
	return Quaternion(0.0f,0.0f,0.0f,1.0f);
}

Quaternion Quaternion::Conjugate(const Quaternion& quaternion)
{
	return Quaternion(-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w);
}

Quaternion Quaternion::Inverse(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	if (norm == 0.0f) {
		// 単位Quaternionを返す
		return Identity();
	}
	Quaternion conjugate = Conjugate(quaternion);
	float norm2 = norm * norm;
	return conjugate / norm2;
}

Quaternion Quaternion::Normalize(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	if (norm == 0.0f) {
		// 単位Quaternionを返す
		return Identity();
	}
	return (Quaternion)quaternion / norm;
}

Quaternion Quaternion::Multiply(const Quaternion& lhs, const Quaternion& rhs){
	Quaternion result;
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
	return result;
}

float Quaternion::Norm(const Quaternion& quaternion)
{
	return sqrtf(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	Quaternion result;
	float halfAngle = angle / 2.0f;
	float sinHalfAngle = sinf(halfAngle);

	// 回転軸を正規化
	Vector3 axisNormal = axis;
	axisNormal = axisNormal.Normalize();

	//クォータニオンを計算
	result.x = axisNormal.x * sinHalfAngle;
	result.y = axisNormal.y * sinHalfAngle;
	result.z = axisNormal.z * sinHalfAngle;
	result.w = cosf(halfAngle);
	return result;
}

Vector3 Quaternion::RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Quaternion vQuaternion = { vector.x, vector.y, vector.z, 0.0f };
	//共役を取得
	Quaternion vConjugate = Quaternion::Conjugate(quaternion);
	//回転
	Quaternion vResult = Quaternion::Multiply(Quaternion::Multiply(quaternion, vQuaternion), vConjugate);
	return Vector3(vResult.x, vResult.y, vResult.z);
}

Matrix4x4 Quaternion::MakeRotateMatrix(const Quaternion& quaternion)
{
	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;
	float ww = quaternion.w * quaternion.w;
	float xy = quaternion.x * quaternion.y;
	float xz = quaternion.x * quaternion.z;
	float yz = quaternion.y * quaternion.z;
	float wx = quaternion.w * quaternion.x;
	float wy = quaternion.w * quaternion.y;
	float wz = quaternion.w * quaternion.z;

	Matrix4x4 result = {
		ww + xx - yy - zz,2.0f * (xy + wz),2.0f * (xz - wy),0.0f,
		2.0f * (xy - wz),ww - xx + yy - zz,2.0f * (yz + wx),0.0f,
		2.0f * (xz + wy),2.0f * (yz - wx),ww - xx - yy + zz,0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}
Matrix4x4 Quaternion::MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate){
	Matrix4x4 m;
	m = Matrix4x4::MakeScaleMatrix(scale) * MakeRotateMatrix(rotate) * Matrix4x4::MakeTranslateMatrix(translate);
	return m;
}
Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, float t){
	Quaternion q0Copy = q0;
	Quaternion q1Copy = q1;
	// q0とq1の内積
	float dot = q0Copy.x * q1Copy.x + q0Copy.y * q1Copy.y + q0Copy.z * q1Copy.z + q0Copy.w * q1Copy.w;

	// 内積が負の場合は反転させて最短経路を取るようにする
	if (dot < 0.0f) {
		q1Copy = -q1Copy;//もう片方の回転を利用
		dot = -dot; //内積も反転
	}

	// 内積が非常に大きい場合、線形補間を行う
	const float EPSILON = 0.0005f;
	if (dot >= 1.0f - EPSILON) {
		// 線形補間
		return q0Copy * (1.0f - t) + q1Copy * t;
	}

	// なす角を求める
	float theta = acosf(dot);

	// 補間係数を計算
	float sinTheta = sinf(theta);
	float scale0 = sinf((1.0f - t) * theta) / sinTheta;
	float scale1 = sinf(t * theta) / sinTheta;

	// 補間されたクォータニオンを計算
	return q0Copy * scale0 + q1Copy * scale1;
}

Quaternion Quaternion::operator+(const Quaternion& q)
{
	Quaternion result;
	result.x = (*this).x + q.x;
	result.y = (*this).y + q.y;
	result.z = (*this).z + q.z;
	result.w = (*this).w + q.w;
	return result;
}

Quaternion Quaternion::operator*(const float& f)
{
	Quaternion result;
	result.x = (*this).x * f;
	result.y = (*this).y * f;
	result.z = (*this).z * f;
	result.w = (*this).w * f;
	return result;
}
Quaternion Quaternion::operator/(const float& f)
{
	Quaternion result;
	result.x = (*this).x / f;
	result.y = (*this).y / f;
	result.z = (*this).z / f;
	result.w = (*this).w / f;
	return result;
}

Quaternion Quaternion::operator-()
{
	Quaternion result;
	result.x = -(*this).x;
	result.y = -(*this).y;
	result.z = -(*this).z;
	result.w = -(*this).w;
	return result;
}