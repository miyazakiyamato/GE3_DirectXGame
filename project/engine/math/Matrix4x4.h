#pragma once
#include <string>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

/// <summary>
/// 4x4行列
/// </summary>
class Matrix4x4{
public:
	//メンバ変数
	float m[4][4];
	//メンバ関数
	static Matrix4x4 Inverse(const Matrix4x4& m);
	static Matrix4x4 Transpose(const Matrix4x4& m);
	static Matrix4x4 MakeIdentity4x4();

	static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
	static Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	static Vector3 Transform(const Vector3& Vector, const Matrix4x4& matrix);
	static Matrix4x4 MakeRotateXMatrix(float radius);
	static Matrix4x4 MakeRotateYMatrix(float radius);
	static Matrix4x4 MakeRotateZMatrix(float radius);

	static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	static Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);


	//演算子のオーバーロード
	Matrix4x4 operator+(const Matrix4x4& m2);
	Matrix4x4 operator-(const Matrix4x4& m2);
	Matrix4x4 operator*(const Matrix4x4& m2);
};
