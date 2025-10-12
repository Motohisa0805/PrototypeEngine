#include "Math.h"

const Vector2 Vector2::Zero(0.0f, 0.0f);
const Vector2 Vector2::UnitX(1.0f, 0.0f);
const Vector2 Vector2::UnitY(0.0f, 1.0f);
const Vector2 Vector2::NegUnitX(-1.0f, 0.0f);
const Vector2 Vector2::NegUnitY(0.0f, -1.0f);

const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::UnitXYZ(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::NegUnitX(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::NegUnitY(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::NegUnitZ(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::Infinity(Math::Infinity, Math::Infinity, Math::Infinity);
const Vector3 Vector3::NegInfinity(Math::NegInfinity, Math::NegInfinity, Math::NegInfinity);

static float m3Ident[3][3] =
{
	{ 1.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f }
};
const Matrix3 Matrix3::Identity(m3Ident);

static float m4Ident[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
};


const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

const Matrix4 Matrix4::Identity(m4Ident);

Vector2 Vector2::Transform(const Vector2& vec, const Matrix3& mat, float w /*= 1.0f*/)
{
	Vector2 retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] + w * mat.mat[2][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] + w * mat.mat[2][1];
	//ignore w since we aren't returning a new value for it...
	return retVal;
}

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
{
	Vector3 temp;
	temp.x = a.y * b.z - a.z * b.y;
	temp.y = a.z * b.x - a.x * b.z;
	temp.z = a.x * b.y - a.y * b.x;
	return temp;
}

Vector3 Vector3::Cross(const Vector3& b) const
{
	Vector3 temp;
	temp.x = y * b.z - z * b.y;
	temp.y = z * b.x - x * b.z;
	temp.z = x * b.y - y * b.x;
	return temp;
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float f)
{
	return Vector3(a + f * (b - a));
}

Vector3 Vector3::LerpXYZ(const Vector3& a, const Vector3& b, float f)
{
	return Vector3(
		Math::Lerp(a.x, b.x, f),
		Math::Lerp(a.y, b.y, f),
		Math::Lerp(a.z, b.z, f)
	);
}

Vector3 Vector3::Reflect(const Vector3& v, const Vector3& n)
{
	return v - 2.0f * Vector3::Dot(v, n) * n;
}

Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
{
	return Vector3(
		std::min(a.x, b.x),
		std::min(a.y, b.y),
		std::min(a.z, b.z)
	);
}

Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
{
	return Vector3(
		std::max(a.x, b.x),
		std::max(a.y, b.y),
		std::max(a.z, b.z)
	);
}

Vector3 Vector3::Transform(const Vector3& vec, const Matrix4& mat, float w /*= 1.0f*/)
{
	Vector3 retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
		vec.z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
		vec.z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
		vec.z * mat.mat[2][2] + w * mat.mat[3][2];
	//ignore w since we aren't returning a new value for it...
	return retVal;
}

// This will transform the vector and renormalize the w component
Vector3 Vector3::TransformWithPerspDiv(const Vector3& vec, const Matrix4& mat, float w /*= 1.0f*/)
{
	Vector3 retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
		vec.z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
		vec.z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
		vec.z * mat.mat[2][2] + w * mat.mat[3][2];
	float transformedW = vec.x * mat.mat[0][3] + vec.y * mat.mat[1][3] +
		vec.z * mat.mat[2][3] + w * mat.mat[3][3];
	if (!Math::NearZero(Math::Abs(transformedW)))
	{
		transformedW = 1.0f / transformedW;
		retVal *= transformedW;
	}
	return retVal;
}

// Transform a Vector3 by a quaternion
Vector3 Vector3::Transform(const Vector3& v, const Quaternion& q)
{
	// v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
	Vector3 qv(q.x, q.y, q.z);
	Vector3 retVal = v;
	retVal += 2.0f * Vector3::Cross(qv, Vector3::Cross(qv, v) + q.w * v);
	return retVal;
}

Vector3 Vector3::Axis(int i)
{
	if (i == 0)
		return Vector3::UnitX;
	else if (i == 1)
		return Vector3::UnitY;
	else
		return Vector3::UnitZ;
	return Vector3::UnitZ;
}



Vector4 Vector4::Normalize(const Vector4& vec)
{
	Vector4 temp = vec;
	temp.Normalize();
	return temp;
}

float Vector4::Dot(const Vector4& a, const Vector4& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, float f)
{
	return Vector4(a + f * (b - a));
}

Vector4 Vector4::Reflect(const Vector4& v, const Vector4& n)
{
	return v - 2.0f * Vector4::Dot(v, n) * n;
}

void Quaternion::Rotate(const Vector3& axis, float angle)
{
	float scalar = Math::Sin(angle / 2.0f);
	x = axis.x * scalar;
	y = axis.y * scalar;
	z = axis.z * scalar;
	w = Math::Cos(angle / 2.0f);
}

Vector3 Quaternion::Rotate(const Vector3& v) const
{
	// q * v * q^-1 を使う方法
	Quaternion qv(v.x, v.y, v.z, 0.0f);
	Quaternion result = (*this) * qv * Inverse();
	return Vector3(result.x, result.y, result.z);
}

float Quaternion::LengthSq() const
{
	return (x * x + y * y + z * z + w * w);
}

float Quaternion::Length() const
{
	return Math::Sqrt(LengthSq());
}

void Quaternion::Normalize()
{
	float length = Length();
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}

Vector3 Quaternion::ToEulerAngles() const
{
	Vector3 angles;

	// X (ピッチ)成分
	// sin(pitch)の計算。Z-X-Y順におけるピッチ角 (X軸) の項
	float sinp = 2.0f * (x * w - z * y);

	// ジンバルロックのチェック (ピッチが +/-90度の場合)
	if (fabs(sinp) >= 1.0f)
	{
		// ジンバルロック状態
		angles.x = std::copysign(Math::Pi / 2.0f, sinp);

		// Z (ヨー)成分 (Z軸周りの回転)
		angles.z = 2.0f * Math::Atan2(z, w);

		// Y (ロール)成分 (Y軸周りの回転)
		angles.y = 0.0f; // Y軸は固定される
	}
	else
	{
		// X (ピッチ) を計算
		angles.x = Math::Asin(sinp); // ★Math::Asin または std::asin を使用

		// Y (ロール)成分
		// atan2( sin(roll)*cos(pitch), cos(roll)*cos(pitch) )
		angles.y = Math::Atan2(2.0f * (y * w + x * z), 1.0f - 2.0f * (y * y + x * x));

		// Z (ヨー)成分
		// atan2( sin(yaw)*cos(pitch), cos(yaw)*cos(pitch) )
		angles.z = Math::Atan2(2.0f * (z * w + x * y), 1.0f - 2.0f * (z * z + x * x));
	}

	// 抽出された角度はラジアンなので、度数に変換してGUIに渡す必要がある
	// 呼び出し側で Math::ToDegrees を使うか、ここで変換してください
	// (通常、この関数はラジアンで返す方が望ましいです)

	return angles;
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
	Quaternion retVal = q;
	retVal.Normalize();
	return retVal;
}

Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float f)
{
	Quaternion retVal;
	retVal.x = Math::Lerp(a.x, b.x, f);
	retVal.y = Math::Lerp(a.y, b.y, f);
	retVal.z = Math::Lerp(a.z, b.z, f);
	retVal.w = Math::Lerp(a.w, b.w, f);
	retVal.Normalize();
	return retVal;
}

float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Quaternion Quaternion::AngleAxis(float angleDegrees, const Vector3& axis)
{
	Vector3 normAxis = axis;
	normAxis.Normalize();

	float radians = angleDegrees * (Math::Pi / 180.0f); // 度→ラジアン変換
	float halfAngle = radians * 0.5f;
	float s = std::sin(halfAngle);
	float c = std::cos(halfAngle);

	return Quaternion(
		normAxis.x * s,
		normAxis.y * s,
		normAxis.z * s,
		c
	);
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float f)
{
	float rawCosm = Quaternion::Dot(a, b);

	float cosom = -rawCosm;
	if (rawCosm >= 0.0f)
	{
		cosom = rawCosm;
	}

	float scale0, scale1;

	if (cosom < 0.9999f)
	{
		const float omega = Math::Acos(cosom);
		const float invSin = 1.f / Math::Sin(omega);
		scale0 = Math::Sin((1.f - f) * omega) * invSin;
		scale1 = Math::Sin(f * omega) * invSin;
	}
	else
	{
		// クォータニオンが共線である場合は、線形補間を使用。
		scale0 = 1.0f - f;
		scale1 = f;
	}

	if (rawCosm < 0.0f)
	{
		scale1 = -scale1;
	}

	Quaternion retVal;
	retVal.x = scale0 * a.x + scale1 * b.x;
	retVal.y = scale0 * a.y + scale1 * b.y;
	retVal.z = scale0 * a.z + scale1 * b.z;
	retVal.w = scale0 * a.w + scale1 * b.w;
	retVal.Normalize();
	return retVal;
}

Quaternion Quaternion::Concatenate(const Quaternion& q, const Quaternion& p)
{
	Quaternion retVal;

	// ベクトル成分:
	// ps * qv + qs * pv + pv x qv
	Vector3 qv(q.x, q.y, q.z);
	Vector3 pv(p.x, p.y, p.z);
	Vector3 newVec = p.w * qv + q.w * pv + Vector3::Cross(pv, qv);
	retVal.x = newVec.x;
	retVal.y = newVec.y;
	retVal.z = newVec.z;

	// スカラー成分:
	// ps * qs - pv . qv
	retVal.w = p.w * q.w - Vector3::Dot(pv, qv);

	return retVal;
}

Vector3 Quaternion::RotateVector(const Vector3 scale, const Quaternion& parent)
{
	// ベクトルをクォータニオン形式に変換
	Quaternion qv = Quaternion(0, scale.x, scale.y, scale.z);
	// 回転を適用
	qv = parent * qv * parent.Inverse();
	// 回転後のベクトルを返す
	return Vector3(qv.x, qv.y, qv.z);
}
//回転させる軸、角度を代入(ラジアンは関数内で変換)
Quaternion Quaternion::CreateFromAxisAngle(const Vector3& axis, float angleDegrees)
{
	// 角度をラジアンに変換
	float angleRadians = Math::ToRadians(angleDegrees);
	// axisが正規化されていることを前提にしているが、念のためNormalizeしてもOK
	Vector3 normAxis = Vector3::Normalize(axis);
	return Quaternion(normAxis, angleRadians);
}

Quaternion Quaternion::LookRotation(const Vector3& forward, const Vector3& up)
{
	Vector3 f = forward.Normalized();
	// 右方向
	Vector3 r = up.Cross(f).Normalized();
	// 上方向（直交系）
	Vector3 u = f.Cross(r);

	// 3x3 回転行列を作る
	float m00 = r.x, m01 = u.x, m02 = f.x;
	float m10 = r.y, m11 = u.y, m12 = f.y;
	float m20 = r.z, m21 = u.z, m22 = f.z;

	float trace = m00 + m11 + m22;
	Quaternion q;

	if (trace > 0.0f) {
		float s = 0.5f / sqrtf(trace + 1.0f);
		q.w = 0.25f / s;
		q.x = (m21 - m12) * s;
		q.y = (m02 - m20) * s;
		q.z = (m10 - m01) * s;
	}
	else {
		if (m00 > m11 && m00 > m22) {
			float s = 2.0f * sqrtf(1.0f + m00 - m11 - m22);
			q.w = (m21 - m12) / s;
			q.x = 0.25f * s;
			q.y = (m01 + m10) / s;
			q.z = (m02 + m20) / s;
		}
		else if (m11 > m22) {
			float s = 2.0f * sqrtf(1.0f + m11 - m00 - m22);
			q.w = (m02 - m20) / s;
			q.x = (m01 + m10) / s;
			q.y = 0.25f * s;
			q.z = (m12 + m21) / s;
		}
		else {
			float s = 2.0f * sqrtf(1.0f + m22 - m00 - m11);
			q.w = (m10 - m01) / s;
			q.x = (m02 + m20) / s;
			q.y = (m12 + m21) / s;
			q.z = 0.25f * s;
		}
	}

	return q.Normalized();
}

float Quaternion::Angle(const Quaternion& a, const Quaternion& b)
{
	float dot = a.Dot(b);
	return std::acos(std::min(std::abs(dot), 1.0f)) * 2.0f; // ラジアン角
}

Quaternion Quaternion::RotateTowards(const Quaternion& from, const Quaternion& to, float maxRadiansDelta)
{
	float angle = Quaternion::Angle(from, to);
	if (angle == 0.0f) return to;

	float t = std::min(1.0f, maxRadiansDelta / angle);
	return Quaternion::Slerp(from, to, t);
}

void Matrix4::Invert()
{
	// Thanks slow math
	// This is a really janky way to unroll everything...
	float tmp[12];
	float src[16];
	float dst[16];
	float det;

	// Transpose matrix
	// row 1 to col 1
	src[0] = mat[0][0];
	src[4] = mat[0][1];
	src[8] = mat[0][2];
	src[12] = mat[0][3];

	// row 2 to col 2
	src[1] = mat[1][0];
	src[5] = mat[1][1];
	src[9] = mat[1][2];
	src[13] = mat[1][3];

	// row 3 to col 3
	src[2] = mat[2][0];
	src[6] = mat[2][1];
	src[10] = mat[2][2];
	src[14] = mat[2][3];

	// row 4 to col 4
	src[3] = mat[3][0];
	src[7] = mat[3][1];
	src[11] = mat[3][2];
	src[15] = mat[3][3];

	// Calculate cofactors
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];

	dst[0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
	dst[0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
	dst[1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
	dst[1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
	dst[2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
	dst[2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
	dst[3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
	dst[3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
	dst[4] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
	dst[4] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
	dst[5] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
	dst[5] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
	dst[6] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
	dst[6] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
	dst[7] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
	dst[7] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];

	tmp[0] = src[2] * src[7];
	tmp[1] = src[3] * src[6];
	tmp[2] = src[1] * src[7];
	tmp[3] = src[3] * src[5];
	tmp[4] = src[1] * src[6];
	tmp[5] = src[2] * src[5];
	tmp[6] = src[0] * src[7];
	tmp[7] = src[3] * src[4];
	tmp[8] = src[0] * src[6];
	tmp[9] = src[2] * src[4];
	tmp[10] = src[0] * src[5];
	tmp[11] = src[1] * src[4];

	dst[8] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
	dst[8] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
	dst[9] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
	dst[9] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
	dst[10] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
	dst[10] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
	dst[11] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
	dst[11] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
	dst[12] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
	dst[12] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
	dst[13] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
	dst[13] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
	dst[14] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
	dst[14] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
	dst[15] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
	dst[15] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];

	// Calculate determinant
	det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];

	// Inverse of matrix is divided by determinant
	det = 1 / det;
	for (int j = 0; j < 16; j++)
	{
		dst[j] *= det;
	}

	// Set it back
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat[i][j] = dst[i * 4 + j];
		}
	}
}

Matrix4 Matrix4::CreateScale(float xScale, float yScale, float zScale)
{
	float temp[4][4] =
	{
		{ xScale, 0.0f, 0.0f, 0.0f },
		{ 0.0f, yScale, 0.0f, 0.0f },
		{ 0.0f, 0.0f, zScale, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreateScale(const Vector3& scale)
{
	return CreateScale(scale.x, scale.y, scale.z);
}

Matrix4 Matrix4::CreateRotationX(float theta)
{
	float temp[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f , 0.0f },
		{ 0.0f, Math::Cos(theta), Math::Sin(theta), 0.0f },
		{ 0.0f, -Math::Sin(theta), Math::Cos(theta), 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f },
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreateRotationY(float theta)
{
	float temp[4][4] =
	{
		{ Math::Cos(theta), 0.0f, -Math::Sin(theta), 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ Math::Sin(theta), 0.0f, Math::Cos(theta), 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f },
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreateRotationZ(float theta)
{
	float temp[4][4] =
	{
		{ Math::Cos(theta), Math::Sin(theta), 0.0f, 0.0f },
		{ -Math::Sin(theta), Math::Cos(theta), 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f },
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreateFromQuaternion(const class Quaternion& q)
{
	float mat[4][4] =
	{
		{1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z , 2.0f * q.x * q.y + 2.0f * q.w * q.z        , 2.0f * q.x * q.z - 2.0f * q.w * q.y        , 0.0f},
		{2.0f * q.x * q.y - 2.0f * q.w * q.z        , 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z , 2.0f * q.y * q.z + 2.0f * q.w * q.x        , 0.0f},
		{2.0f * q.x * q.z + 2.0f * q.w * q.y        , 2.0f * q.y * q.z - 2.0f * q.w * q.x        , 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y , 0.0f},
		{0.0f                                       , 0.0f                                       , 0.0f                                       , 1.0f},
	};
	return Matrix4(mat);
}

Matrix4 Matrix4::CreateTranslation(const Vector3& trans)
{
	float temp[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ trans.x, trans.y, trans.z, 1.0f }
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	Vector3 forward = Vector3::Normalize(target - eye);
	Vector3 right = Vector3::Normalize(Vector3::Cross(up, forward));
	Vector3 newUp = Vector3::Normalize(Vector3::Cross(forward, right));
	Vector3 trans;
	trans.x = -Vector3::Dot(right, eye);
	trans.y = -Vector3::Dot(newUp, eye);
	trans.z = -Vector3::Dot(forward, eye);

	float temp[4][4] =
	{
		{ right.x, newUp.x, forward.x, 0.0f },
		{ right.y, newUp.y, forward.y, 0.0f },
		{ right.z, newUp.z, forward.z, 0.0f },
		{ trans.x, trans.y, trans.z, 1.0f }
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreateOrtho(float width, float height, float near_SDL, float far_SDL)
{
	float temp[4][4] =
	{
		{ 2.0f / width, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / height, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f / (far_SDL - near_SDL), 0.0f },
		{ 0.0f, 0.0f, near_SDL / (near_SDL - far_SDL), 1.0f }
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreatePerspectiveFOV(float fovY, float width, float height, float near_SDL, float far_SDL)
{
	float yScale = Math::Cot(fovY / 2.0f);
	float xScale = yScale * height / width;
	float temp[4][4] =
	{
		{ xScale, 0.0f, 0.0f, 0.0f },
		{ 0.0f, yScale, 0.0f, 0.0f },
		{ 0.0f, 0.0f, far_SDL / (far_SDL - near_SDL), 1.0f },
		{ 0.0f, 0.0f, -near_SDL * far_SDL / (far_SDL - near_SDL), 0.0f }
	};
	return Matrix4(temp);
}

Matrix4 Matrix4::CreateSimpleViewProj(float width, float height)
{
	float temp[4][4] =
	{
		{ 2.0f / width, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / height, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f }
	};
	return Matrix4(temp);
}

Quaternion Matrix4::FromMatrix(const Matrix4& mat)
{
	Quaternion q;
	float trace = mat.mat[0][0] + mat.mat[1][1] + mat.mat[2][2];

	if (trace > 0.0f)
	{
		float s = Math::Sqrt(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (mat.mat[2][1] - mat.mat[1][2]) / s;
		q.y = (mat.mat[0][2] - mat.mat[2][0]) / s;
		q.z = (mat.mat[1][0] - mat.mat[0][1]) / s;
	}
	else
	{
		if (mat.mat[0][0] > mat.mat[1][1] && mat.mat[0][0] > mat.mat[2][2])
		{
			float s = Math::Sqrt(1.0f + mat.mat[0][0] - mat.mat[1][1] - mat.mat[2][2]) * 2.0f;
			q.w = (mat.mat[2][1] - mat.mat[1][2]) / s;
			q.x = 0.25f * s;
			q.y = (mat.mat[0][1] + mat.mat[1][0]) / s;
			q.z = (mat.mat[0][2] + mat.mat[2][0]) / s;
		}
		else if (mat.mat[1][1] > mat.mat[2][2])
		{
			float s = Math::Sqrt(1.0f + mat.mat[1][1] - mat.mat[0][0] - mat.mat[2][2]) * 2.0f;
			q.w = (mat.mat[0][2] - mat.mat[2][0]) / s;
			q.x = (mat.mat[0][1] + mat.mat[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (mat.mat[1][2] + mat.mat[2][1]) / s;
		}
		else
		{
			float s = Math::Sqrt(1.0f + mat.mat[2][2] - mat.mat[0][0] - mat.mat[1][1]) * 2.0f;
			q.w = (mat.mat[1][0] - mat.mat[0][1]) / s;
			q.x = (mat.mat[0][2] + mat.mat[2][0]) / s;
			q.y = (mat.mat[1][2] + mat.mat[2][1]) / s;
			q.z = 0.25f * s;
		}
	}

	return q;
}

Matrix4 Matrix4::Billboard(const Vector3& pos, const float& size, const Vector3& camRight, const Vector3& camUp, const Vector3& camForward)
{
	Matrix4 billboard = Matrix4::Identity;

	billboard.mat[0][0] = camRight.x * size;
	billboard.mat[0][1] = camRight.y * size;
	billboard.mat[0][2] = camRight.z * size;

	billboard.mat[1][0] = camUp.x * size;
	billboard.mat[1][1] = camUp.y * size;
	billboard.mat[1][2] = camUp.z * size;

	billboard.mat[2][0] = camForward.x * size;
	billboard.mat[2][1] = camForward.y * size;
	billboard.mat[2][2] = camForward.z * size;

	// 位置設定
	billboard.mat[3][0] = pos.x;
	billboard.mat[3][1] = pos.y;
	billboard.mat[3][2] = pos.z;
	return billboard;
}

Matrix3 Matrix3::CreateScale(float xScale, float yScale)
{
	float temp[3][3] =
	{
		{ xScale, 0.0f, 0.0f },
		{ 0.0f, yScale, 0.0f },
		{ 0.0f, 0.0f, 1.0f },
	};
	return Matrix3(temp);
}

Matrix3 Matrix3::CreateScale(const Vector2& scaleVector)
{
	return CreateScale(scaleVector.x, scaleVector.y);
}

Matrix3 Matrix3::CreateScale(float scale)
{
	return CreateScale(scale, scale);
}

Matrix3 Matrix3::CreateRotation(float theta)
{
	float temp[3][3] =
	{
		{ Math::Cos(theta), Math::Sin(theta), 0.0f },
		{ -Math::Sin(theta), Math::Cos(theta), 0.0f },
		{ 0.0f, 0.0f, 1.0f },
	};
	return Matrix3(temp);
}

Matrix3 Matrix3::CreateTranslation(const Vector2& trans)
{
	float temp[3][3] =
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ trans.x, trans.y, 1.0f },
	};
	return Matrix3(temp);
}
