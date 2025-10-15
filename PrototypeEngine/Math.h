#pragma once
#include "StandardLibrary.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍元のコードを改造したファイル
//Vector2、Vector3、Matrix関係の計算処理を行っているnamespace
namespace Math
{
	const float Pi = 3.14159274f;
	const float TwoPi = Pi * 2.0f;
	const float PiOver2 = Pi / 2.0f;
	const float Infinity = std::numeric_limits<float>::infinity();
	const float NegInfinity = -std::numeric_limits<float>::infinity();

	inline float ToRadians(float degrees)
	{
		return degrees * (Pi / 180.0f);
	}

	inline float ToDegrees(float radians)
	{
		return radians * (180.0f / Pi);
	}

	inline bool NearZero(float val, float epsilon = 0.001f)
	{
		if (fabs(val) <= epsilon)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename T>
	T Max(const T& a, const T& b)
	{
		return (a < b ? b : a);
	}

	template <typename T>
	T Min(const T& a, const T& b)
	{
		return (a < b ? a : b);
	}

	template <typename T>
	T Clamp(const T& value, const T& lower, const T& upper)
	{
		return Min(upper, Max(lower, value));
	}

	inline float Abs(float value)
	{
		return fabs(value);
	}

	inline float Cos(float angle)
	{
		return cosf(angle);
	}

	inline float Sin(float angle)
	{
		return sinf(angle);
	}

	inline float Asin(float f) 
	{
		return std::asin(f); 
	} 

	inline float Tan(float angle)
	{
		return tanf(angle);
	}

	inline float Acos(float value)
	{
		return acosf(value);
	}

	inline float Atan2(float y, float x)
	{
		return atan2f(y, x);
	}

	inline float Cot(float angle)
	{
		return 1.0f / Tan(angle);
	}

	inline float Lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	inline float Sqrt(float value)
	{
		return sqrtf(value);
	}

	inline float Fmod(float numer, float denom)
	{
		return fmod(numer, denom);
	}

	inline int Sign(float value)
	{
		if (value > 0) return 1;
		if (value < 0) return -1;
		return 0;
	}
}

// 2D Vector
class Vector2
{
public:
	float x;
	float y;

	Vector2()
		:x(0.0f)
		, y(0.0f)
	{
	}

	explicit Vector2(float inX, float inY)
		:x(inX)
		, y(inY)
	{
	}

	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&x);
	}

	// 両方のコンポーネントを1行に設定
	void Set(float inX, float inY)
	{
		x = inX;
		y = inY;
	}

	// ベクトル加算（a + b）
	friend Vector2 operator+(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x + b.x, a.y + b.y);
	}

	// ベクトル減算（a - b）
	friend Vector2 operator-(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x - b.x, a.y - b.y);
	}

	// 要素ごとの乗算
	// (a.x * b.x, ...)
	friend Vector2 operator*(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x * b.x, a.y * b.y);
	}

	// スカラー乗算
	friend Vector2 operator*(const Vector2& vec, float scalar)
	{
		return Vector2(vec.x * scalar, vec.y * scalar);
	}

	// スカラー乗算
	friend Vector2 operator*(float scalar, const Vector2& vec)
	{
		return Vector2(vec.x * scalar, vec.y * scalar);
	}

	// Scalar *=
	Vector2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	// Vector +=
	Vector2& operator+=(const Vector2& right)
	{
		x += right.x;
		y += right.y;
		return *this;
	}

	// Vector -=
	Vector2& operator-=(const Vector2& right)
	{
		x -= right.x;
		y -= right.y;
		return *this;
	}

	// ベクトルの長さの二乗
	float LengthSq() const
	{
		return (x * x + y * y);
	}

	// ベクトルの長さ
	float Length() const
	{
		return (Math::Sqrt(LengthSq()));
	}

	// ベクトルを正規化
	void Normalize()
	{
		float length = Length();
		x /= length;
		y /= length;
	}

	// 渡されたベクトルを正規化
	static Vector2 Normalize(const Vector2& vec)
	{
		Vector2 temp = vec;
		temp.Normalize();
		return temp;
	}

	// 二つのベクトル間のドット積（a・b）
	static float Dot(const Vector2& a, const Vector2& b)
	{
		return (a.x * b.x + a.y * b.y);
	}

	// AからBへfによって線形補間
	static Vector2 Lerp(const Vector2& a, const Vector2& b, float f)
	{
		return Vector2(a + f * (b - a));
	}

	// N（正規化された）について反射
	static Vector2 Reflect(const Vector2& v, const Vector2& n)
	{
		return v - 2.0f * Vector2::Dot(v, n) * n;
	}

	// 行列によってベクトルを変換する
	static Vector2 Transform(const Vector2& vec, const class Matrix3& mat, float w = 1.0f);

	static const Vector2 Zero;
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 NegUnitX;
	static const Vector2 NegUnitY;
};

// 3D Vector
class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3()
		:x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{
	}

	explicit Vector3(float inX, float inY, float inZ)
		:x(inX)
		, y(inY)
		, z(inZ)
	{
	}

	// const float ポインタにキャスト
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&x);
	}

	// すべての三つのコンポーネントを1行に設定
	void Set(float inX, float inY, float inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}

	// ベクトル加算（a + b）
	friend Vector3 operator+(const Vector3& a, const Vector3& b)
	{
		return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	friend Vector3 operator+(const Vector3& a, float b)
	{
		return Vector3(a.x + b, a.y + b, a.z + b);
	}

	// ベクトル減算（a - b）
	friend Vector3 operator-(const Vector3& a, const Vector3& b)
	{
		return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	friend Vector3 operator-(const Vector3& a, float b)
	{
		return Vector3(a.x - b, a.y - b, a.z - b);
	}

	// ベクトル減算（a - b）
	friend Vector3 operator/(const Vector3& a, const Vector3& b)
	{
		return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
	}

	// 要素ごとの乗算
	friend Vector3 operator*(const Vector3& left, const Vector3& right)
	{
		return Vector3(left.x * right.x, left.y * right.y, left.z * right.z);
	}

	// スカラー乗算
	friend Vector3 operator*(const Vector3& vec, float scalar)
	{
		return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	friend Vector3 operator/(const Vector3& vec, float scalar)
	{
		return Vector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
	}

	// スカラー乗算
	friend Vector3 operator*(float scalar, const Vector3& vec)
	{
		return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	// Scalar *=
	Vector3& operator*=(Vector3 scalar)
	{
		x *= scalar.x;
		y *= scalar.y;
		z *= scalar.z;
		return *this;
	}

	Vector3& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// Vector +=
	Vector3& operator+=(Vector3 right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		return *this;
	}

	// Vector +=
	Vector3& operator+=(float right)
	{
		x += right;
		y += right;
		z += right;
		return *this;
	}

	// Vector -=
	Vector3& operator-=(const Vector3& right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
		return *this;
	}

	Vector3& operator-=(float right)
	{
		x -= right;
		y -= right;
		z -= right;
		return *this;
	}

	static Vector3 Abs(Vector3 value)
	{
		return Vector3(fabs(value.x), fabs(value.y), fabs(value.z));
	}

	// ベクトルの長さの二乗
	float LengthSq() const
	{
		return (x * x + y * y + z * z);
	}

	// ベクトルの長さ
	float Length() const
	{
		return (Math::Sqrt(LengthSq()));
	}

	// ベクトルの正規化
	void Normalize()
	{
		float length = Length();
		if (length > 0.000001f) // もしくは std::numeric_limits<float>::epsilon() を使っても良い
		{
			x /= length;
			y /= length;
			z /= length;
		}
		else
		{
			// 長さが0に近い場合はゼロベクトルにするなど安全な処理をする
			x = y = z = 0.0f;
		}
	}


	// 渡されたベクトルを正規化
	static Vector3 Normalize(const Vector3& vec)
	{
		Vector3 temp = vec;
		temp.Normalize();
		return temp;
	}

	// 渡されたベクトルを正規化
	Vector3 Normalized() const
	{
		Vector3 temp = Vector3(x,y,z);
		temp.Normalize();
		return temp;
	}

	// 二つのベクトル間のドット積（a・b）
	static float Dot(const Vector3& a, const Vector3& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	// 二つのベクトルのクロス積（a × b）
	static Vector3 Cross(const Vector3& a, const Vector3& b);

	// 二つのベクトルのクロス積（a × b）
	Vector3 Cross(const Vector3& b) const;

	// AからBへfによって線形補間
	static Vector3 Lerp(const Vector3& a, const Vector3& b, float f);

	static Vector3 LerpXYZ(const Vector3& a, const Vector3& b, float f);

	// N（正規化された）について反射する
	static Vector3 Reflect(const Vector3& v, const Vector3& n);

	// 成分ごとのMin
	static Vector3 Min(const Vector3& a, const Vector3& b);

	// 成分ごとのMax
	static Vector3 Max(const Vector3& a, const Vector3& b);

	static Vector3 Transform(const Vector3& vec, const class Matrix4& mat, float w = 1.0f);
	// ベクトルを変換、w成分が再正規化
	static Vector3 TransformWithPerspDiv(const Vector3& vec, const class Matrix4& mat, float w = 1.0f);

	// クォータニオンでベクトル3を変換
	static Vector3 Transform(const Vector3& v, const class Quaternion& q);

	static Vector3 Axis(int i);

	static const Vector3 Zero;
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 UnitXYZ;
	static const Vector3 NegUnitX;
	static const Vector3 NegUnitY;
	static const Vector3 NegUnitZ;
	static const Vector3 Infinity;
	static const Vector3 NegInfinity;
};

inline bool operator<(const Vector3& lhs, const Vector3& rhs)
{
	if (lhs.x != rhs.x) return lhs.x < rhs.x;
	if (lhs.y != rhs.y) return lhs.y < rhs.y;
	return lhs.z < rhs.z;
}

// 4D Vector
class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4()
		:x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, w(0.0f)
	{
	}

	explicit Vector4(float inX, float inY, float inZ, float inW)
		:x(inX)
		, y(inY)
		, z(inZ)
		, w(inW)
	{
	}

	// const float ポインタにキャスト
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&x);
	}

	// すべての三つのコンポーネントを1行に設定
	void Set(float inX, float inY, float inZ, float inW)
	{
		x = inX;
		y = inY;
		z = inZ;
		w = inW;
	}

	// ベクトル加算（a + b）
	friend Vector4 operator+(const Vector4& a, const Vector4& b)
	{
		return Vector4(a.x + b.x, a.y + b.y, a.z + b.z,a.w + b.w);
	}

	// ベクトル減算 (a - b)
	friend Vector4 operator-(const Vector4& a, const Vector4& b)
	{
		return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	// 要素ごとの乗算
	friend Vector4 operator*(const Vector4& left, const Vector4& right)
	{
		return Vector4(left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w);
	}

	// スカラー乗算
	friend Vector4 operator*(const Vector4& vec, float scalar)
	{
		return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
	}

	// スカラー乗算
	friend Vector4 operator*(float scalar, const Vector4& vec)
	{
		return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
	}

	// Scalar *=
	Vector4& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	// Vector +=
	Vector4& operator+=(const Vector4& right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		w += right.w;
		return *this;
	}

	// Vector -=
	Vector4& operator-=(const Vector4& right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
		w -= right.w;
		return *this;
	}

	// ベクトルの長さの二乗
	float LengthSq() const
	{
		return (x * x + y * y + z * z + w * w);
	}

	// ベクトルの長さ
	float Length() const
	{
		return (Math::Sqrt(LengthSq()));
	}

	// ベクトルを正規化
	void Normalize()
	{
		float length = Length();
		x /= length;
		y /= length;
		z /= length;
		w /= length;
	}

	// 渡されたベクトルを正規化
	static Vector4 Normalize(const Vector4& vec);

	// 二つのベクトル間のドット積（a・b）
	static float Dot(const Vector4& a, const Vector4& b);

	// AからBへfによって線形補間
	static Vector4 Lerp(const Vector4& a, const Vector4& b, float f);

	// N（正規化された）について反射
	static Vector4 Reflect(const Vector4& v, const Vector4& n);

	static const Vector4 Zero;
	static const Vector4 UnitX;
	static const Vector4 UnitY;
	static const Vector4 UnitZ;
	static const Vector4 NegUnitX;
	static const Vector4 NegUnitY;
	static const Vector4 NegUnitZ;
	static const Vector4 Infinity;
	static const Vector4 NegInfinity;
};

// 3x3 Matrix
class Matrix3
{
public:
	float mat[3][3];

	Matrix3()
	{
		*this = Matrix3::Identity;
	}

	explicit Matrix3(float inMat[3][3])
	{
		memcpy(mat, inMat, 9 * sizeof(float));
	}

	// const floatポインタにキャスト
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&mat[0][0]);
	}

	// 行列の乗算
	friend Matrix3 operator*(const Matrix3& left, const Matrix3& right)
	{
		Matrix3 retVal;
		// row 0
		retVal.mat[0][0] =
			left.mat[0][0] * right.mat[0][0] +
			left.mat[0][1] * right.mat[1][0] +
			left.mat[0][2] * right.mat[2][0];

		retVal.mat[0][1] =
			left.mat[0][0] * right.mat[0][1] +
			left.mat[0][1] * right.mat[1][1] +
			left.mat[0][2] * right.mat[2][1];

		retVal.mat[0][2] =
			left.mat[0][0] * right.mat[0][2] +
			left.mat[0][1] * right.mat[1][2] +
			left.mat[0][2] * right.mat[2][2];

		// row 1
		retVal.mat[1][0] =
			left.mat[1][0] * right.mat[0][0] +
			left.mat[1][1] * right.mat[1][0] +
			left.mat[1][2] * right.mat[2][0];

		retVal.mat[1][1] =
			left.mat[1][0] * right.mat[0][1] +
			left.mat[1][1] * right.mat[1][1] +
			left.mat[1][2] * right.mat[2][1];

		retVal.mat[1][2] =
			left.mat[1][0] * right.mat[0][2] +
			left.mat[1][1] * right.mat[1][2] +
			left.mat[1][2] * right.mat[2][2];

		// row 2
		retVal.mat[2][0] =
			left.mat[2][0] * right.mat[0][0] +
			left.mat[2][1] * right.mat[1][0] +
			left.mat[2][2] * right.mat[2][0];

		retVal.mat[2][1] =
			left.mat[2][0] * right.mat[0][1] +
			left.mat[2][1] * right.mat[1][1] +
			left.mat[2][2] * right.mat[2][1];

		retVal.mat[2][2] =
			left.mat[2][0] * right.mat[0][2] +
			left.mat[2][1] * right.mat[1][2] +
			left.mat[2][2] * right.mat[2][2];

		return retVal;
	}

	Matrix3& operator*=(const Matrix3& right)
	{
		*this = *this * right;
		return *this;
	}

	// xおよびyスケールを持つスケール行列を作成
	static Matrix3 CreateScale(float xScale, float yScale);

	static Matrix3 CreateScale(const Vector2& scaleVector);

	// 均一な因子でスケール行列を作成
	static Matrix3 CreateScale(float scale);

	// Z軸回りの回転行列を作成
	// シータはラジアンで表されます
	static Matrix3 CreateRotation(float theta);

	// xy平面上に翻訳行列を作成
	static Matrix3 CreateTranslation(const Vector2& trans);

	static const Matrix3 Identity;
};

// (Unit) Quaternion
class Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion()
	{
		*this = Quaternion::Identity;
	}

	// これは四元数の成分を直接設定します -- 軸/角度には使用しない
	explicit Quaternion(float inX, float inY, float inZ, float inW)
	{
		Set(inX, inY, inZ, inW);
	}
	// 軸と角度からクォータニオンを構築します。
	// 軸はすでに正規化されていると仮定し、
	// 角度はラジアンで表されます。
	explicit Quaternion(const Vector3& axis, float angle)
	{
		Rotate(axis, angle);
	}

	// 内部コンポーネントを直接設定する
	void Set(float inX, float inY, float inZ, float inW)
	{
		x = inX;
		y = inY;
		z = inZ;
		w = inW;
	}

	void Conjugate()
	{
		x *= -1.0f;
		y *= -1.0f;
		z *= -1.0f;
	}

	void Rotate(const Vector3& axis, float angle);

	Vector3 Rotate(const Vector3& v) const;

	float LengthSq() const;

	float Length() const;

	void Normalize();

	Vector3 ToEulerAngles()const;

	// 提供された四元数を正規化
	static Quaternion Normalize(const Quaternion& q);

	// 線形補間
	static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float f);

	static float Dot(const Quaternion& a, const Quaternion& b);

	inline float Dot(const Quaternion& b) const
	{
		return x * b.x + y * b.y + z * b.z + w * b.w;
	}

	static Quaternion AngleAxis(float angleDegrees, const Vector3& axis);

	// 球面線形補間
	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float f);

	// 連結
	// qで回転した後にpを続ける
	static Quaternion Concatenate(const Quaternion& q, const Quaternion& p);

	// 四元数の逆行列を計算するメソッド
	inline Quaternion Inverse() const 
	{
		// ノルムの2乗
		float normSquared = x * x + y * y + z * z + w * w;
		if (normSquared == 0) 
		{
			// ゼロ除算を防ぐため、逆行列は存在しないとする場合
			// もしくは例外を投げる等
			return Quaternion(0, 0, 0, 0);  
		}

		// 共役を取る（虚部を反転させる）
		return Quaternion(-x / normSquared, -y / normSquared, -z / normSquared, w / normSquared);
	}

	static Vector3 RotateVector(const Vector3 scale, const Quaternion& parent);

	inline Vector3 RotateVector(const Vector3& v) const
	{
		// q * v * q^-1 を実装している
		Quaternion qv(v.x, v.y, v.z, 1.0f);
		Quaternion result = (*this) * qv * Inverse();
		return Vector3(result.x, result.y, result.z);
	}


	// 指定軸と角度で回転クォータニオンを作成して返す関数（角度は度数）
	static Quaternion CreateFromAxisAngle(const Vector3& axis, float angleDegrees);

	// 自身の回転に、指定軸と角度の回転を追加する（角度は度数）
	inline void RotateByAxisAngle(const Vector3& axis, float angleDegrees)
	{
		float angleRadians = Math::ToRadians(angleDegrees);
		// 追加回転を生成
		Quaternion deltaRot(axis, angleRadians); 
		// deltaRotを先に掛ける（後から回転）
		*this = deltaRot * (*this);              
	}

	inline Quaternion Normalized() const 
	{
		float mag = sqrtf(x * x + y * y + z * z + w * w);
		// 単位クォータニオンを返す
		if (mag == 0.0f) return Quaternion(0, 0, 0, 1); 
		return Quaternion(x / mag, y / mag, z / mag, w / mag);
	}

	static Quaternion LookRotation(const Vector3& forward, const Vector3& up = Vector3(0, 1, 0));

	static float Angle(const Quaternion& a, const Quaternion& b);

	static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxRadiansDelta);

	static const Quaternion Identity;

	Quaternion& operator*=(const Quaternion& q)
	{
		float newX = w * q.x + x * q.w + y * q.z - z * q.y;
		float newY = w * q.y - x * q.z + y * q.w + z * q.x;
		float newZ = w * q.z + x * q.y - y * q.x + z * q.w;
		float newW = w * q.w - x * q.x - y * q.y - z * q.z;

		x = newX;
		y = newY;
		z = newZ;
		w = newW;

		return *this;
	}

	

	friend Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion(
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
			q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
		);
	}
};
// 4x4 Matrix
//FOCUS : 行列は行優先で作成
class Matrix4
{
public:
	float mat[4][4];

	Matrix4()
	{
		*this = Matrix4::Identity;
	}

	explicit Matrix4(float inMat[4][4])
	{
		memcpy(mat, inMat, 16 * sizeof(float));
	}

	// const float ポインタにキャスト
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&mat[0][0]);
	}

	// 行列の掛け算 (a * b)
	friend Matrix4 operator*(const Matrix4& a, const Matrix4& b)
	{
		Matrix4 retVal;
		// 行 0
		retVal.mat[0][0] =
			a.mat[0][0] * b.mat[0][0] +
			a.mat[0][1] * b.mat[1][0] +
			a.mat[0][2] * b.mat[2][0] +
			a.mat[0][3] * b.mat[3][0];

		retVal.mat[0][1] =
			a.mat[0][0] * b.mat[0][1] +
			a.mat[0][1] * b.mat[1][1] +
			a.mat[0][2] * b.mat[2][1] +
			a.mat[0][3] * b.mat[3][1];

		retVal.mat[0][2] =
			a.mat[0][0] * b.mat[0][2] +
			a.mat[0][1] * b.mat[1][2] +
			a.mat[0][2] * b.mat[2][2] +
			a.mat[0][3] * b.mat[3][2];

		retVal.mat[0][3] =
			a.mat[0][0] * b.mat[0][3] +
			a.mat[0][1] * b.mat[1][3] +
			a.mat[0][2] * b.mat[2][3] +
			a.mat[0][3] * b.mat[3][3];

		// 行 1
		retVal.mat[1][0] =
			a.mat[1][0] * b.mat[0][0] +
			a.mat[1][1] * b.mat[1][0] +
			a.mat[1][2] * b.mat[2][0] +
			a.mat[1][3] * b.mat[3][0];

		retVal.mat[1][1] =
			a.mat[1][0] * b.mat[0][1] +
			a.mat[1][1] * b.mat[1][1] +
			a.mat[1][2] * b.mat[2][1] +
			a.mat[1][3] * b.mat[3][1];

		retVal.mat[1][2] =
			a.mat[1][0] * b.mat[0][2] +
			a.mat[1][1] * b.mat[1][2] +
			a.mat[1][2] * b.mat[2][2] +
			a.mat[1][3] * b.mat[3][2];

		retVal.mat[1][3] =
			a.mat[1][0] * b.mat[0][3] +
			a.mat[1][1] * b.mat[1][3] +
			a.mat[1][2] * b.mat[2][3] +
			a.mat[1][3] * b.mat[3][3];

		// 行 2
		retVal.mat[2][0] =
			a.mat[2][0] * b.mat[0][0] +
			a.mat[2][1] * b.mat[1][0] +
			a.mat[2][2] * b.mat[2][0] +
			a.mat[2][3] * b.mat[3][0];

		retVal.mat[2][1] =
			a.mat[2][0] * b.mat[0][1] +
			a.mat[2][1] * b.mat[1][1] +
			a.mat[2][2] * b.mat[2][1] +
			a.mat[2][3] * b.mat[3][1];

		retVal.mat[2][2] =
			a.mat[2][0] * b.mat[0][2] +
			a.mat[2][1] * b.mat[1][2] +
			a.mat[2][2] * b.mat[2][2] +
			a.mat[2][3] * b.mat[3][2];

		retVal.mat[2][3] =
			a.mat[2][0] * b.mat[0][3] +
			a.mat[2][1] * b.mat[1][3] +
			a.mat[2][2] * b.mat[2][3] +
			a.mat[2][3] * b.mat[3][3];

		// 行 3
		retVal.mat[3][0] =
			a.mat[3][0] * b.mat[0][0] +
			a.mat[3][1] * b.mat[1][0] +
			a.mat[3][2] * b.mat[2][0] +
			a.mat[3][3] * b.mat[3][0];

		retVal.mat[3][1] =
			a.mat[3][0] * b.mat[0][1] +
			a.mat[3][1] * b.mat[1][1] +
			a.mat[3][2] * b.mat[2][1] +
			a.mat[3][3] * b.mat[3][1];

		retVal.mat[3][2] =
			a.mat[3][0] * b.mat[0][2] +
			a.mat[3][1] * b.mat[1][2] +
			a.mat[3][2] * b.mat[2][2] +
			a.mat[3][3] * b.mat[3][2];

		retVal.mat[3][3] =
			a.mat[3][0] * b.mat[0][3] +
			a.mat[3][1] * b.mat[1][3] +
			a.mat[3][2] * b.mat[2][3] +
			a.mat[3][3] * b.mat[3][3];

		return retVal;
	}

	Matrix4& operator*=(const Matrix4& right)
	{
		*this = *this * right;
		return *this;
	}

	// 行列を反転させる
	void Invert();

	// 行列の翻訳成分を取得する
	inline Vector3 GetTranslation() const
	{
		return Vector3(mat[3][0], mat[3][1], mat[3][2]);
	}

	// 行列のX軸を取得する（前方）
	inline Vector3 GetXAxis() const
	{
		return Vector3::Normalize(Vector3(mat[0][0], mat[0][1], mat[0][2]));
	}

	// 行列のY軸を取得してください（左）
	inline Vector3 GetYAxis() const
	{
		return Vector3::Normalize(Vector3(mat[1][0], mat[1][1], mat[1][2]));
	}

	// 行列のZ軸を取得する（上）
	inline Vector3 GetZAxis() const
	{
		return Vector3::Normalize(Vector3(mat[2][0], mat[2][1], mat[2][2]));
	}

	// 行列からスケール成分を抽出
	inline Vector3 GetScale() const
	{
		Vector3 retVal;
		retVal.x = Vector3(mat[0][0], mat[0][1], mat[0][2]).Length();
		retVal.y = Vector3(mat[1][0], mat[1][1], mat[1][2]).Length();
		retVal.z = Vector3(mat[2][0], mat[2][1], mat[2][2]).Length();
		return retVal;
	}
	//行優先の行列を列優先の行列に変換
	// 行列の転置を返す
	// 列優先の行列を返す
	inline Quaternion GetRotation() const
	{
		Quaternion q;
		float trace = mat[0][0] + mat[1][1] + mat[2][2];

		if (trace > 0.0f)
		{
			float s = sqrtf(trace + 1.0f) * 2.0f;
			q.w = 0.25f * s;
			q.x = (mat[1][2] - mat[2][1]) / s;
			q.y = (mat[2][0] - mat[0][2]) / s;
			q.z = (mat[0][1] - mat[1][0]) / s;
		}
		else
		{
			if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2])
			{
				float s = sqrtf(1.0f + mat[0][0] - mat[1][1] - mat[2][2]) * 2.0f;
				q.w = (mat[1][2] - mat[2][1]) / s;
				q.x = 0.25f * s;
				q.y = (mat[1][0] + mat[0][1]) / s;
				q.z = (mat[2][0] + mat[0][2]) / s;
			}
			else if (mat[1][1] > mat[2][2])
			{
				float s = sqrtf(1.0f + mat[1][1] - mat[0][0] - mat[2][2]) * 2.0f;
				q.w = (mat[2][0] - mat[0][2]) / s;
				q.x = (mat[1][0] + mat[0][1]) / s;
				q.y = 0.25f * s;
				q.z = (mat[2][1] + mat[1][2]) / s;
			}
			else
			{
				float s = sqrtf(1.0f + mat[2][2] - mat[0][0] - mat[1][1]) * 2.0f;
				q.w = (mat[0][1] - mat[1][0]) / s;
				q.x = (mat[2][0] + mat[0][2]) / s;
				q.y = (mat[2][1] + mat[1][2]) / s;
				q.z = 0.25f * s;
			}
		}

		return q;
	}
	// 行列からスケールを除去した新しい行列を返す
	// 列優先の行列を返す
	inline Matrix4 RemoveScale()const
	{
		// 各軸を抽出
		Vector3 xAxis = Vector3(mat[0][0], mat[0][1], mat[0][2]);
		Vector3 yAxis = Vector3(mat[1][0], mat[1][1], mat[1][2]);
		Vector3 zAxis = Vector3(mat[2][0], mat[2][1], mat[2][2]);

		// 長さを取得（スケール）
		float scaleX = xAxis.Length();
		float scaleY = yAxis.Length();
		float scaleZ = zAxis.Length();

		// 正規化された軸で新しい行列を作成
		Matrix4 retVal = *this;

		// スケールを考慮して軸成分を正規化
		// (xAxis / scaleX)
		retVal.mat[0][0] /= scaleX;
		retVal.mat[0][1] /= scaleX;
		retVal.mat[0][2] /= scaleX;

		// (yAxis / scaleY)
		retVal.mat[1][0] /= scaleY;
		retVal.mat[1][1] /= scaleY;
		retVal.mat[1][2] /= scaleY;

		// (zAxis / scaleZ)
		retVal.mat[2][0] /= scaleZ;
		retVal.mat[2][1] /= scaleZ;
		retVal.mat[2][2] /= scaleZ;

		return retVal;
	}

	// x、y、zのスケールを持つスケール行列を作成
	//行優先の行列を作成
	static Matrix4 CreateScale(float xScale, float yScale, float zScale);
	// x、y、zのスケールを持つスケール行列を作成
	//行優先の行列を作成
	static Matrix4 CreateScale(const Vector3& scaleVector);
	// 均一な因子でスケール行列を作成
	//行優先の行列を作成
	static Matrix4 CreateScale(float scale)
	{
		return CreateScale(scale, scale, scale);
	}
	// x軸回転
	//列優先の行列を作成
	static Matrix4 CreateRotationX(float theta);
	// y軸回転
	//列優先の行列を作成
	static Matrix4 CreateRotationY(float theta);
	// z軸回転
	//列優先の行列を作成
	static Matrix4 CreateRotationZ(float theta);
	// クォータニオンから回転行列を作成
	//列優先の行列を作成
	static Matrix4 CreateFromQuaternion(const class Quaternion& q);
	// Positionを取得(列優先)
	static Matrix4 CreateTranslation(const Vector3& trans);
	//カメラがある位置（eye）から、特定のターゲット（target）を見るような視点変換行列を行う
	//列優先の行列を作成
	static Matrix4 CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
	//カメラがある位置（eye）から、特定のターゲット（target）を見るような視点変換行列を行う
	//列優先の行列を作成
	static Matrix4 CreateOrtho(float width, float height, float near_SDL, float far_SDL);
	//透視投影行列（Perspective Projection Matrix）を作成する関数
	//3D空間のシーンを、カメラを通して「遠くの物は小さく見える」ようなパース（遠近感）付きの2D画面に投影する行列を構築
	//列優先の行列を作成
	static Matrix4 CreatePerspectiveFOV(float fovY, float width, float height, float near_SDL, float far_SDL);
	// シンプルビュー投影行列を作成する
	// これは、カメラの位置を原点に、X軸とY軸をそれぞれ幅と高さでスケーリングするだけの行列
	//列優先の行列を作成
	static Matrix4 CreateSimpleViewProj(float width, float height);
	//現在未使用
	static Quaternion FromMatrix(const Matrix4& mat);
	//列優先の行列を作成
	static Matrix4 Billboard(const Vector3& pos, const float& size, const Vector3& camRight, const Vector3& camUp, const Vector3& camForward);

	static const Matrix4 Identity;
};


namespace Color
{
	static const Vector3 Black(0.0f, 0.0f, 0.0f);
	static const Vector3 White(1.0f, 1.0f, 1.0f);
	static const Vector3 Red(1.0f, 0.0f, 0.0f);
	static const Vector3 Green(0.0f, 1.0f, 0.0f);
	static const Vector3 Blue(0.0f, 0.0f, 1.0f);
	static const Vector3 Yellow(1.0f, 1.0f, 0.0f);
	static const Vector3 LightYellow(1.0f, 1.0f, 0.88f);
	static const Vector3 LightBlue(0.68f, 0.85f, 0.9f);
	static const Vector3 LightPink(1.0f, 0.71f, 0.76f);
	static const Vector3 LightGreen(0.56f, 0.93f, 0.56f);
	static const Vector4 mClearColor(0.5f, 0.7f, 1.0f, 1.0f);
}
