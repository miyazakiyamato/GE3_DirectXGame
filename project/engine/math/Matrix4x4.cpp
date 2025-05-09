#include "Matrix4x4.h"
#include <cmath>
#include <cassert>

Matrix4x4 Matrix4x4::Inverse()
{
	return Inverse(*this);
}

Matrix4x4 Matrix4x4::Transpose()
{
	return Transpose(*this);
}

Vector3 Matrix4x4::Transform(const Vector3& Vector)
{
	return Transform(Vector,*this);
}

Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& m) {
	Matrix4x4 m2;
	float a =
		m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	m2.m[0][0] = 1.0f / a * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
	m2.m[0][1] = 1.0f / a * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	m2.m[0][2] = 1.0f / a * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	m2.m[0][3] = 1.0f / a * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	m2.m[1][0] = 1.0f / a * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
	m2.m[1][1] = 1.0f / a * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
	m2.m[1][2] = 1.0f / a * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
	m2.m[1][3] = 1.0f / a * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

	m2.m[2][0] = 1.0f / a * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
	m2.m[2][1] = 1.0f / a * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
	m2.m[2][2] = 1.0f / a * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	m2.m[2][3] = 1.0f / a * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	m2.m[3][0] = 1.0f / a * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
	m2.m[3][1] = 1.0f / a * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
	m2.m[3][2] = 1.0f / a * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
	m2.m[3][3] = 1.0f / a * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return m2;
}

Matrix4x4 Matrix4x4::Transpose(const Matrix4x4& m) {
	Matrix4x4 m2;
	m2.m[0][0] = m.m[0][0]; m2.m[0][1] = m.m[1][0]; m2.m[0][2] = m.m[2][0]; m2.m[0][3] = m.m[3][0];
	m2.m[1][0] = m.m[0][1]; m2.m[1][1] = m.m[1][1]; m2.m[1][2] = m.m[2][1]; m2.m[1][3] = m.m[3][1];
	m2.m[2][0] = m.m[0][2]; m2.m[2][1] = m.m[1][2]; m2.m[2][2] = m.m[2][2]; m2.m[2][3] = m.m[3][2];
	m2.m[3][0] = m.m[0][3]; m2.m[3][1] = m.m[1][3]; m2.m[3][2] = m.m[2][3]; m2.m[3][3] = m.m[3][3];
	return m2;
}
Matrix4x4 Matrix4x4::MakeIdentity4x4()
{
	Matrix4x4 identity = {
		.m{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
	  }
	};
	return identity;
}
Matrix4x4 Matrix4x4::MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 m;
	m.m[0][0] = 1; m.m[0][1] = 0; m.m[0][2] = 0; m.m[0][3] = 0;
	m.m[1][0] = 0; m.m[1][1] = 1; m.m[1][2] = 0; m.m[1][3] = 0;
	m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = 1; m.m[2][3] = 0;
	m.m[3][0] = translate.x; m.m[3][1] = translate.y; m.m[3][2] = translate.z; m.m[3][3] = 1;
	return m;
}

Matrix4x4 Matrix4x4::MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 m;
	m.m[0][0] = scale.x; m.m[0][1] = 0; m.m[0][2] = 0; m.m[0][3] = 0;
	m.m[1][0] = 0; m.m[1][1] = scale.y; m.m[1][2] = 0; m.m[1][3] = 0;
	m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = scale.z; m.m[2][3] = 0;
	m.m[3][0] = 0; m.m[3][1] = 0; m.m[3][2] = 0; m.m[3][3] = 1;
	return m;
}

Vector3 Matrix4x4::Transform(const Vector3& Vector, const Matrix4x4& matrix) {
	Vector3 v;
	v.x = Vector.x * matrix.m[0][0] + Vector.y * matrix.m[1][0] + Vector.z * matrix.m[2][0] + matrix.m[3][0];
	v.y = Vector.x * matrix.m[0][1] + Vector.y * matrix.m[1][1] + Vector.z * matrix.m[2][1] + matrix.m[3][1];
	v.z = Vector.x * matrix.m[0][2] + Vector.y * matrix.m[1][2] + Vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = Vector.x * matrix.m[0][3] + Vector.y * matrix.m[1][3] + Vector.z * matrix.m[2][3] + matrix.m[3][3];

	assert(w != 0.0f);
	v.x /= w;
	v.y /= w;
	v.z /= w;

	return v;
}

Matrix4x4 Matrix4x4::MakeRotateXMatrix(float radius) {
	Matrix4x4 m;
	m.m[0][0] = 1; m.m[0][1] = 0; m.m[0][2] = 0; m.m[0][3] = 0;
	m.m[1][0] = 0; m.m[1][1] = std::cosf(radius); m.m[1][2] = std::sinf(radius); m.m[1][3] = 0;
	m.m[2][0] = 0; m.m[2][1] = -std::sinf(radius); m.m[2][2] = std::cosf(radius);  m.m[2][3] = 0;
	m.m[3][0] = 0; m.m[3][1] = 0; m.m[3][2] = 0; m.m[3][3] = 1;
	return m;
}

Matrix4x4 Matrix4x4::MakeRotateYMatrix(float radius) {
	Matrix4x4 m;
	m.m[0][0] = std::cosf(radius); m.m[0][1] = 0; m.m[0][2] = -std::sinf(radius); m.m[0][3] = 0;
	m.m[1][0] = 0; m.m[1][1] = 1; m.m[1][2] = 0; m.m[1][3] = 0;
	m.m[2][0] = std::sinf(radius); m.m[2][1] = 0; m.m[2][2] = std::cosf(radius); m.m[2][3] = 0;
	m.m[3][0] = 0; m.m[3][1] = 0; m.m[3][2] = 0; m.m[3][3] = 1;
	return m;
}

Matrix4x4 Matrix4x4::MakeRotateZMatrix(float radius) {
	Matrix4x4 m;
	m.m[0][0] = std::cosf(radius); m.m[0][1] = std::sinf(radius); m.m[0][2] = 0; m.m[0][3] = 0;
	m.m[1][0] = -std::sinf(radius); m.m[1][1] = std::cosf(radius); m.m[1][2] = 0; m.m[1][3] = 0;
	m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = 1; m.m[2][3] = 0;
	m.m[3][0] = 0; m.m[3][1] = 0; m.m[3][2] = 0; m.m[3][3] = 1;
	return m;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 m;
	Matrix4x4 rotateMatrix = MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);
	m = MakeScaleMatrix(scale) * rotateMatrix * MakeTranslateMatrix(translate);
	return m;
}

Matrix4x4 Matrix4x4::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 m;
	m.m[0][0] = 1.0f / aspectRatio * (1.0f / std::tanf(fovY / 2.0f)); m.m[0][1] = 0; m.m[0][2] = 0; m.m[0][3] = 0;
	m.m[1][0] = 0; m.m[1][1] = 1.0f / std::tanf(fovY / 2.0f); m.m[1][2] = 0; m.m[1][3] = 0;
	m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = farClip / (farClip - nearClip); m.m[2][3] = 1;
	m.m[3][0] = 0; m.m[3][1] = 0; m.m[3][2] = -nearClip * farClip / (farClip - nearClip); m.m[3][3] = 0;
	return m;
}

Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 m;
	m.m[0][0] = 2.0f / (right - left); m.m[0][1] = 0; m.m[0][2] = 0; m.m[0][3] = 0;
	m.m[1][0] = 0; m.m[1][1] = 2.0f / (top - bottom); m.m[1][2] = 0; m.m[1][3] = 0;
	m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = 1.0f / (farClip - nearClip); m.m[2][3] = 0;
	m.m[3][0] = (left + right) / (left - right); m.m[3][1] = (top + bottom) / (bottom - top); m.m[3][2] = nearClip / (nearClip - farClip); m.m[3][3] = 1;
	return m;
}

Matrix4x4 Matrix4x4::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 m;
	m.m[0][0] = width / 2.0f; m.m[0][1] = 0; m.m[0][2] = 0; m.m[0][3] = 0;
	m.m[1][0] = 0; m.m[1][1] = -height / 2.0f; m.m[1][2] = 0; m.m[1][3] = 0;
	m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = maxDepth - minDepth; m.m[2][3] = 0;
	m.m[3][0] = left + (width / 2.0f); m.m[3][1] = top + (height / 2.0f); m.m[3][2] = minDepth; m.m[3][3] = 1;
	return m;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& m2)
{
	Matrix4x4 m3, m1 = (*this);
	m3.m[0][0] = m1.m[0][0] + m2.m[0][0]; m3.m[0][1] = m1.m[0][1] + m2.m[0][1]; m3.m[0][2] = m1.m[0][2] + m2.m[0][2]; m3.m[0][3] = m1.m[0][3] + m2.m[0][3];
	m3.m[1][0] = m1.m[1][0] + m2.m[1][0]; m3.m[1][1] = m1.m[1][1] + m2.m[1][1]; m3.m[1][2] = m1.m[1][2] + m2.m[1][2]; m3.m[1][3] = m1.m[1][3] + m2.m[1][3];
	m3.m[2][0] = m1.m[2][0] + m2.m[2][0]; m3.m[2][1] = m1.m[2][1] + m2.m[2][1]; m3.m[2][2] = m1.m[2][2] + m2.m[2][2]; m3.m[2][3] = m1.m[2][3] + m2.m[2][3];
	m3.m[3][0] = m1.m[3][0] + m2.m[3][0]; m3.m[3][1] = m1.m[3][1] + m2.m[3][1]; m3.m[3][2] = m1.m[3][2] + m2.m[3][2]; m3.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return m3;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& m2)
{
	Matrix4x4 m3 ,m1 = (*this);
	m3.m[0][0] = m1.m[0][0] - m2.m[0][0]; m3.m[0][1] = m1.m[0][1] - m2.m[0][1]; m3.m[0][2] = m1.m[0][2] - m2.m[0][2]; m3.m[0][3] = m1.m[0][3] - m2.m[0][3];
	m3.m[1][0] = m1.m[1][0] - m2.m[1][0]; m3.m[1][1] = m1.m[1][1] - m2.m[1][1]; m3.m[1][2] = m1.m[1][2] - m2.m[1][2]; m3.m[1][3] = m1.m[1][3] - m2.m[1][3];
	m3.m[2][0] = m1.m[2][0] - m2.m[2][0]; m3.m[2][1] = m1.m[2][1] - m2.m[2][1]; m3.m[2][2] = m1.m[2][2] - m2.m[2][2]; m3.m[2][3] = m1.m[2][3] - m2.m[2][3];
	m3.m[3][0] = m1.m[3][0] - m2.m[3][0]; m3.m[3][1] = m1.m[3][1] - m2.m[3][1]; m3.m[3][2] = m1.m[3][2] - m2.m[3][2]; m3.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return m3;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m2)
{
	Matrix4x4 m3, m1 = (*this);
	m3.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0]; m3.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1]; m3.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2]; m3.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	m3.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0]; m3.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1]; m3.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2]; m3.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	m3.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0]; m3.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1]; m3.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2]; m3.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	m3.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0]; m3.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1]; m3.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2]; m3.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return m3;
}

