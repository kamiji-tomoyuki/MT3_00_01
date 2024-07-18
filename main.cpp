#include <cmath>
#define _USE_MATH_DEFINES
#include <Novice.h>
#include <imgui.h>
#include "Vector/Vector3.h"
#include "Vector/Matrix4x4.h"
#include "assert.h"
#include <algorithm>

const char kWindowTitle[] = "LE2B_07_カミジ_トモユキ";


struct Sphere {
	Vector3 center;//中心点
	float radius;  //半径
};

struct Line {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};

struct Ray {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};

struct Segment {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};

struct Plane {
	Vector3 normal; //法線
	float distance; //距離
};

struct Triangle {
	Vector3 vertices[3]; //頂点
};

struct AABB {
	Vector3 min;//最小点
	Vector3 max;//最大店
};

struct Spring
{
	Vector3 anchor;           // 固定位置
	float naturalLength;      // 自然長
	float stiffness;          // バネ定数
	float dampingCoefficient; // 減衰係数
};

struct Ball
{
	Vector3 position;     
	Vector3 velocity;     
	Vector3 acceleration; 
	float mass;// 質量
	float radius;         
	unsigned int color;   
};

///////////////////////////////////////////////////////////////////////

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate)
{
	Matrix4x4 result;

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1;

	return result;
}
//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale)
{
	Matrix4x4 result;

	result.m[0][0] = scale.x;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
//回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {

	Matrix4x4 result;

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 result;

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 result;

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = 0;
	result.m[0][2] = -std::sin(radian);
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = std::sin(radian);
	result.m[2][1] = 0;
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
// 内積
float Dot(const Vector3& v1, const Vector3& v2)
{
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}

	return result;
}
//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}

	return result;
}
//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	float buf;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			buf = 0;
			for (int k = 0; k < 4; k++) {
				buf = buf + m1.m[i][k] * m2.m[k][j];
				result.m[i][j] = buf;
			}
		}
	}

	return result;
}
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};

	Matrix4x4 a{};
	a.m[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2];

	a.m[0][1] = -(m.m[0][1] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2])
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2];

	a.m[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2];

	a.m[0][3] = -(m.m[0][1] * m.m[1][2] * m.m[2][3]) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2])
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2];


	a.m[1][0] = -(m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2])
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2];

	a.m[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2];

	a.m[1][2] = -(m.m[0][0] * m.m[1][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2])
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2];

	a.m[1][3] = m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2];


	a.m[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1];

	a.m[2][1] = -(m.m[0][0] * m.m[2][1] * m.m[3][3]) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1])
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1];

	a.m[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1];

	a.m[2][3] = -(m.m[0][0] * m.m[1][1] * m.m[2][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1])
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1];


	a.m[3][0] = -(m.m[1][0] * m.m[2][1] * m.m[3][2]) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1])
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1];

	a.m[3][1] = m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1];

	a.m[3][2] = -(m.m[0][0] * m.m[1][1] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1])
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1];

	a.m[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1];

	float number;
	number = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])
		- (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])
		- (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	number = 1.0f / number;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[j][i] = number * a.m[j][i];
		}
	}

	return result;

}
//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};
	Matrix4x4 tmp{};
	tmp = m;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = tmp.m[j][i];
		}
	}

	return result;
}
//単位行列
Matrix4x4 MakeIdentity() {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1;
	}

	return result;
}

//Affine
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 S = MakeScaleMatrix(scale);
	Matrix4x4 Rx = MakeRotateXMatrix(rotate.x);
	Matrix4x4 Ry = MakeRotateYMatrix(rotate.y);
	Matrix4x4 Rz = MakeRotateZMatrix(rotate.z);
	Matrix4x4 R = Multiply(Rx, Multiply(Ry, Rz));
	Matrix4x4 T = MakeTranslateMatrix(translate);

	Matrix4x4 W = Multiply(S, Multiply(R, T));

	return W;
}

//////////////////////////////////////////////////////////////////////

//正射影行列4x4
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result;

	result.m[0][0] = 2 / (right - left);
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 2 / (top - bottom);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1 / (farClip - nearClip);
	result.m[2][3] = 0;

	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1;

	return result;
};
//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result;

	result.m[0][0] = (1 / aspectRatio) * (1 / tan(fovY / 2));
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = (1 / tan(fovY / 2));
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = -(nearClip * farClip) / (farClip - nearClip);
	result.m[3][3] = 0;

	return result;
}
//ビューポート行列4x4
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;

	result.m[0][0] = width / 2;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = (height / 2) * -1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0;

	result.m[3][0] = left + (width / 2);
	result.m[3][1] = top + (height / 2);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1;

	return result;
};
//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
};

///////////////////////////////////////////////////////////////////////

//座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}
Vector3 GetWorldPosition(Matrix4x4& worldMatrix)
{
	//ワールド座標を入れる変数
	Vector3 worldPos;

	//ワールド座標の平行移動成分を取得(ワールド座標)
	worldPos.x = worldMatrix.m[3][0];
	worldPos.y = worldMatrix.m[3][1];
	worldPos.z = worldMatrix.m[3][2];

	return worldPos;
}

///////////////////////////////////////////////////////////////////////

//Vector3
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x; result.y = v1.y - v2.y; result.z = v1.z - v2.z;
	return result;
}
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x; result.y = v1.y + v2.y; result.z = v1.z + v2.z;
	return result;
}
Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}
Vector3 Normalize(const Vector3& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return { v.x / length, v.y / length, v.z / length };
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	float t = Dot(v1, v2) / (sqrtf(powf(Dot(v2, v2), 2)));

	result = Multiply(t, v2);

	return result;
}
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 proj = Project(Subtract(point, segment.origin), Subtract(Add(segment.origin, segment.diff), segment.origin));
	Vector3 cp = Add(segment.origin, proj);

	return cp;
}

//距離の算出
float Length(const Vector3& v1, const Vector3& v2) {
	Vector3 distance;
	float d;

	distance = Subtract(v2, v1);
	d = sqrtf(powf(distance.x, 2) + powf(distance.y, 2) + powf(distance.z, 2));

	return d;
}
float Length(const Vector3& v){
	float result;
	result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}
///////////////////////////////////////////////////////////////////////

//描画用関数

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix)
{
	for (int row = 0; row < 4; ++row)
	{
		for (int column = 0; column < 4; ++column)
		{
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label)
{
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

///////////////////////////////////////////////////////////////////////

//グリッドの描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

//球の描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

//平面の描画
Vector3 Perpendicular(const Vector3& vector)
{
	if (vector.x != 0.0f || vector.y != 0.0f)
	{
		return { -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
bool isColision(const Sphere& sphere, const Plane& plane)
{
	float k = sqrtf(powf(Dot(plane.normal, sphere.center) - plane.distance, 2));

	if (k <= sphere.radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//三角形の描画
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

//AABB(箱)の描画
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

//ベジエ曲線の描画
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 result;

	result = Add(v1, Multiply(t, Subtract(v2, v1)));

	return result;
}
Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t)
{
	Vector3 p0p1 = Lerp(p0, p1, t);
	Vector3 p1p2 = Lerp(p1, p2, t);
	Vector3 p = Lerp(p0p1, p1p2, t);

	return p;
}
void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

///////////////////////////////////////////////////////////////////////

//当たり判定
//球 & 球
bool isColision(const Sphere& s1, const Sphere& s2) {
	float add = s1.radius + s2.radius;
	Vector3 distanceVector = { s1.center.x - s2.center.x ,s1.center.y - s2.center.y,s1.center.z - s2.center.z };
	float distance = sqrtf(powf(distanceVector.x, 2) + powf(distanceVector.y, 2) + powf(distanceVector.z, 2));

	if (distance <= add) {
		return true;
	}
	else {
		return false;
	}
}
//線 & 面
bool isColision(const Segment& segment, const Plane& plane) {
	//法線と線の内積
	float dot = Dot(plane.normal, segment.diff);

	//垂直=平行なので衝突しない
	if (dot == 0.0f) { return false; }

	//tを求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	//衝突判定
	if (t <= 1 && t >= 0) {
		return true;
	}
	else {
		return false;
	}
}
//線 & 三角形
bool isColision(const Segment& segment, const Triangle& triangle) {
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);

	Vector3 n = Normalize(Cross(v01, v12));

	float d = Dot(triangle.vertices[0], n);

	// 法線と線の内積
	float dot = Dot(n, segment.diff);

	// 線が平面と平行な場合は衝突しない
	if (fabs(dot) < FLT_EPSILON) {
		return false;
	}

	float t = (d - Dot(segment.origin, n)) / dot;

	// tが線分の範囲内にない場合は衝突しない
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	Vector3 p = Add(segment.origin, Multiply(t, segment.diff));

	Vector3 v0p = Subtract(p, triangle.vertices[0]);
	Vector3 v1p = Subtract(p, triangle.vertices[1]);
	Vector3 v2p = Subtract(p, triangle.vertices[2]);

	// 各辺を結んだベクトル、頂点、衝突点pを結んだベクトルのクロス積を取る
	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	// 衝突判定
	if (Dot(cross01, n) >= 0.0f && Dot(cross12, n) >= 0.0f && Dot(cross20, n) >= 0.0f) {
		return true;
	}
	else {
		return false;
	}
}
//箱 & 箱
bool isColision(const AABB& aabb1, const AABB& aabb2) {

	// 衝突判定
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && //X軸
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && //Y軸
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) { //Z軸
		return true;
	}
	else {
		return false;
	}
};
//箱 & 球
bool isColision(const AABB& aabb, const Sphere& sphere) {
	//最接点を求める
	Vector3 closestPoint{
	 std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
	 std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
	 std::clamp(sphere.center.z,aabb.min.z,aabb.max.z) };

	//最接点と球の中心との距離を求める
	float distance = Length(closestPoint, sphere.center);

	//距離が半径よりも小さければ衝突
	if (distance < sphere.radius) {
		return true;
	}
	else {
		return false;
	}
}
//箱 & 線
//AABB・線
bool isColision(const AABB& aabb, const Segment segment)
{
	Vector3 nX = { 1,0,0 };
	Vector3 nY = { 0,1,0 };
	Vector3 nZ = { 0,0,1 };

	float dotX = Dot(nX, segment.diff);
	float dotY = Dot(nY, segment.diff);
	float dotZ = Dot(nZ, segment.diff);

	float txMin = (aabb.min.x - segment.origin.x) / dotX;
	float txMax = (aabb.max.x - segment.origin.x) / dotX;
	float tyMin = (aabb.min.y - segment.origin.y) / dotY;
	float tyMax = (aabb.max.y - segment.origin.y) / dotY;
	float tzMin = (aabb.min.z - segment.origin.z) / dotZ;
	float tzMax = (aabb.max.z - segment.origin.z) / dotZ;

	float tNearX = min(txMin, txMax);
	float tFarX = max(txMin, txMax);
	float tNearY = min(tyMin, tyMax);
	float tFarY = max(tyMin, tyMax);
	float tNearZ = min(tzMin, tzMax);
	float tFarZ = max(tzMin, tzMax);

	//AABBとの衝突点（貫通点）のtが小さい方
	float tmin = max(max(tNearX, tNearY), tNearZ);

	//AABBとの衝突点（貫通点）のtが大きい方
	float tmax = min(min(tFarX, tFarY), tFarZ);

	if (txMax > INFINITY or txMin < -INFINITY or
		tyMax > INFINITY or tyMin < -INFINITY or
		tzMax > INFINITY or tzMin < -INFINITY)
	{
		return false;
	}

	if (std::isnan(txMax) or std::isnan(txMin) or
		std::isnan(tyMax) or std::isnan(tyMin) or
		std::isnan(tzMax) or std::isnan(tzMin))
	{
		return false;
	}

	//衝突判定
	if (tmin <= tmax)
	{
		if ((tmax <= 1 && tmax >= 0) or (tmin <= 1 && tmin >= 0))
		{
			return true;
		}
		else if ((tFarX >= 1 && tNearX <= 0) &&
			(tFarY >= 1 && tNearY <= 0) &&
			(tFarZ >= 1 && tNearZ <= 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

}

///////////////////////////////////////////////////////////////////////

//演算子オーバーロード
// ヘッダーで宣言済み
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subtract(m1, m2); }
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }
Vector3 operator-(const Vector3& v) { return { -v.x,-v.y,-v.z }; }
Vector3 operator+(const Vector3& v) { return v; }


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Sphere sphere
	{
		{0,0,0},
		0.03f
	};

	Vector3 center = { 0.0f,0,0 };
	float radius = 1.0f;

	float angularVelocity = 3.14f;
	float angle = 0.0f;

	float deltaTime = 1.0f / 60.0f;
	bool isStart = false;

	// カメラ
	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	float cameraSpeed = 0.01f;

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓カメラ
		///

		if (keys[DIK_W]) { cameraTranslate.z += cameraSpeed; }
		if (keys[DIK_S]) { cameraTranslate.z -= cameraSpeed; }

		if (keys[DIK_A]) { cameraTranslate.x -= cameraSpeed; }
		if (keys[DIK_D]) { cameraTranslate.x += cameraSpeed; }

		if (keys[DIK_UP]) { cameraTranslate.y += cameraSpeed; }
		if (keys[DIK_DOWN]) { cameraTranslate.y -= cameraSpeed; }

		Matrix4x4 camelaMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatriix = Inverse(camelaMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, (float)kWindowWidth / (float)kWindowHeight, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatriix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, (float)kWindowWidth, (float)kWindowHeight, 0.0f, 1.0f);

		///
		/// ↓更新処理ここから
		///

		if (isStart) {
			angle += angularVelocity * deltaTime;
		}

		sphere.center.x = center.x + std::cos(angle) * radius;
		sphere.center.y = center.y + std::sin(angle) * radius;
		sphere.center.z = center.z;


		//ImGui
		ImGui::Begin("window");
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		if (ImGui::Button("start")) {
			isStart = true;
		}
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, WHITE);
		
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	float pi = float(3.141592653589);
	const uint32_t kSubdivision = 10;
	const float kLatEvery = pi / kSubdivision;
	const float kLonEvery = (2 * pi) / kSubdivision;

	Vector3 a, b, c;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		sphere;
		float lat = -pi / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			//ワールド座標系での頂点を求める
			a = {
				sphere.radius * std::cos(lat) * std::cos(lon),
				sphere.radius * std::sin(lat),
				sphere.radius * std::cos(lat) * std::sin(lon)
			};

			b = {
				sphere.radius * std::cos(lat + (pi / kSubdivision)) * std::cos(lon),
				sphere.radius * std::sin(lat + (pi / kSubdivision)),
				sphere.radius * std::cos(lat + (pi / kSubdivision)) * std::sin(lon)
			};

			c = {
				sphere.radius * std::cos(lat) * std::cos(lon + ((pi * 2) / kSubdivision)),
				sphere.radius * std::sin(lat),
				sphere.radius * std::cos(lat) * std::sin(lon + ((pi * 2) / kSubdivision))
			};

			//a,b,cをScreen座標系まで変換
			Matrix4x4 worldMatrixA = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, sphere.center);
			Matrix4x4 worldMatrixB = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, sphere.center);
			Matrix4x4 worldMatrixC = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, sphere.center);

			Matrix4x4 wvpMatrixA = Multiply(worldMatrixA, viewProjectionMatrix);
			Matrix4x4 wvpMatrixB = Multiply(worldMatrixB, viewProjectionMatrix);
			Matrix4x4 wvpMatrixC = Multiply(worldMatrixC, viewProjectionMatrix);

			Vector3 localA = Transform(a, wvpMatrixA);
			Vector3 localB = Transform(b, wvpMatrixB);
			Vector3 localC = Transform(c, wvpMatrixC);

			Vector3 screenA = Transform(localA, viewportMatrix);
			Vector3 screenB = Transform(localB, viewportMatrix);
			Vector3 screenC = Transform(localC, viewportMatrix);

			//ab,bcで線を引く
			Novice::DrawLine((int)screenA.x, (int)screenA.y, (int)screenB.x, (int)screenB.y, color);
			Novice::DrawLine((int)screenA.x, (int)screenA.y, (int)screenC.x, (int)screenC.y, color);

		}
	}
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	Vector3 zLineStart;
	Vector3 zLineEnd;
	Vector3 xLineStart;
	Vector3 xLineEnd;

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {

		zLineStart = Vector3(xIndex * kGridEvery / 2 - kGridHalfWidth + 1, 0, 1);
		zLineEnd = Vector3(xIndex * kGridEvery / 2 - kGridHalfWidth + 1, 0, -3);

		//スクリーン座標系まで変換をかける
		Matrix4x4 startWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, zLineStart);
		Matrix4x4 endWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, zLineEnd);

		Matrix4x4 startwvpMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endwvpMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);

		Vector3 startLocal = Transform(zLineStart, startwvpMatrix);
		Vector3 endLocal = Transform(zLineEnd, endwvpMatrix);

		Vector3 startScreen = Transform(startLocal, viewportMatrix);
		Vector3 endScreen = Transform(endLocal, viewportMatrix);

		//変換した座標を使って表示、色は薄い灰色(0xAAAAAAFF)。原点は黒
		if (xIndex == kSubdivision / 2)
		{
			Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, BLACK);
		}
		else
		{
			Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, 0xAAAAAAFF);
		}

	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		xLineStart = Vector3(1, 0, zIndex * kGridEvery / 2 - kGridHalfWidth + 1);
		xLineEnd = Vector3(-1, 0, zIndex * kGridEvery / 2 - kGridHalfWidth + 1);

		//スクリーン座標系まで変換をかける
		Matrix4x4 startWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, xLineStart);
		Matrix4x4 endWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, xLineEnd);

		Matrix4x4 startwvpMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endwvpMatrix = Multiply(endWorldMatrix, viewProjectionMatrix);

		Vector3 startLocal = Transform(xLineStart, startwvpMatrix);
		Vector3 endLocal = Transform(xLineEnd, endwvpMatrix);

		Vector3 startScreen = Transform(startLocal, viewportMatrix);
		Vector3 endScreen = Transform(endLocal, viewportMatrix);

		//変換した座標を使って表示、色は薄い灰色(0xAAAAAAFF)。原点は黒
		if (zIndex == kSubdivision / 2)
		{
			Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, BLACK);
		}
		else
		{
			Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, 0xAAAAAAFF);
		}
	}

}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	Vector3 center = Multiply(plane.distance, plane.normal); // 1
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal)); // 2
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z }; // 3
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]); // 4
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z }; // 5

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index)
	{
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}
	//points をそれぞれ結んでDraw で矩形を描画
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x, (int)points[2].y, color);
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[3].x, (int)points[3].y, color);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[2].x, (int)points[2].y, color);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[3].x, (int)points[3].y, color);
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	//スクリーン座標に変換
	Vector3 transform[3];
	Vector3 screen[3];

	for (int i = 0; i < 3; ++i) {
		transform[i] = Transform(triangle.vertices[i], viewProjectionMatrix);
		screen[i] = Transform(transform[i], viewportMatrix);
	}

	//三角形の描画
	Novice::DrawTriangle((int)screen[0].x, (int)screen[0].y, (int)screen[1].x, (int)screen[1].y, (int)screen[2].x, (int)screen[2].y, color, kFillModeWireFrame);
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	//頂点
	Vector3 corners[8] = {
	   {aabb.min.x, aabb.min.y, aabb.min.z},//左下前
	   {aabb.max.x, aabb.min.y, aabb.min.z},//右下前
	   {aabb.min.x, aabb.max.y, aabb.min.z},//左上前
	   {aabb.max.x, aabb.max.y, aabb.min.z},//右上前
	   {aabb.min.x, aabb.min.y, aabb.max.z},//左下奥
	   {aabb.max.x, aabb.min.y, aabb.max.z},//右下奥
	   {aabb.min.x, aabb.max.y, aabb.max.z},//左上奥
	   {aabb.max.x, aabb.max.y, aabb.max.z}	//右上奥
	};

	Vector3 p[8];//変換後の頂点
	for (int i = 0; i < 8; ++i) {
		Vector3 transform = Transform(corners[i], viewProjectionMatrix);
		Vector3 screen = Transform(transform, viewportMatrix);
		p[i] = screen;
	}

	//辺の描画
	Novice::DrawLine((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y, color);//下前
	Novice::DrawLine((int)p[2].x, (int)p[2].y, (int)p[3].x, (int)p[3].y, color);//上前
	Novice::DrawLine((int)p[0].x, (int)p[0].y, (int)p[2].x, (int)p[2].y, color);//左前
	Novice::DrawLine((int)p[1].x, (int)p[1].y, (int)p[3].x, (int)p[3].y, color);//右前

	Novice::DrawLine((int)p[4].x, (int)p[4].y, (int)p[5].x, (int)p[5].y, color);//下奥
	Novice::DrawLine((int)p[6].x, (int)p[6].y, (int)p[7].x, (int)p[7].y, color);//上奥
	Novice::DrawLine((int)p[4].x, (int)p[4].y, (int)p[6].x, (int)p[6].y, color);//左奥
	Novice::DrawLine((int)p[5].x, (int)p[5].y, (int)p[7].x, (int)p[7].y, color);//右奥

	Novice::DrawLine((int)p[0].x, (int)p[0].y, (int)p[4].x, (int)p[4].y, color);//左下
	Novice::DrawLine((int)p[1].x, (int)p[1].y, (int)p[5].x, (int)p[5].y, color);//右下
	Novice::DrawLine((int)p[2].x, (int)p[2].y, (int)p[6].x, (int)p[6].y, color);//左上
	Novice::DrawLine((int)p[3].x, (int)p[3].y, (int)p[7].x, (int)p[7].y, color);//右上

}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	float num = 32;//分割数

	for (int i = 0; i < num; ++i)
	{
		float t0 = i / float(num);
		float t1 = (i + 1) / float(num);

		Vector3 bezier0 = Bezier(controlPoint0, controlPoint1, controlPoint2, t0);
		Vector3 bezier1 = Bezier(controlPoint0, controlPoint1, controlPoint2, t1);

		Vector3 transform0 = Transform(bezier0, viewProjectionMatrix);
		Vector3 transform1 = Transform(bezier1, viewProjectionMatrix);

		Vector3 screenB0 = Transform(transform0, viewportMatrix);
		Vector3 screenB1 = Transform(transform1, viewportMatrix);

		Novice::DrawLine((int)screenB0.x, (int)screenB0.y, (int)screenB1.x, (int)screenB1.y, color);
	}
}
