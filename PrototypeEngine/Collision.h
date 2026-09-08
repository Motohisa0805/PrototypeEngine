#pragma once
#include "Math.h"
#include "Typedefs.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分を一部改造したファイル
//各当たり判定の構造体をまとめたファイル
//ボックス、球、線など
struct LineSegment
{
					LineSegment(const Vector3& start, const Vector3& end);
	// 0 <= t <= 1 の間のセグメントに沿った点を取得する
	Vector3			PointOnSegment(float t) const;
	// 点と線分の最小距離の二乗を取得する
	float			MinDistSq(const Vector3& point) const;
	// 2つの線分間の最小距離の2乗を取得する
	static float	MinDistSq(const LineSegment& s1, const LineSegment& s2);

	Vector3			sStart;
	Vector3			sEnd;
};

struct Plane
{
			Plane(const Vector3& normal, float d);
	// 3つの点から平面を構築する
			Plane(const Vector3& a, const Vector3& b, const Vector3& c);
	// 点と平面の間の符号付き距離を取得する
	float	SignedDist(const Vector3& point) const;

	Vector3 sNormal;
	float	sD;
};

struct Sphere
{
				Sphere(const Vector3& center, float radius);
	bool		Contains(const Vector3& point) const;

	Vector3		sCenter;
	float		sRadius;
};

struct AABB
{
				AABB(const Vector3& min, const Vector3& max);
	// このポイントを考慮して最小値と最大値を更新する
	// (モデルを読み込むときに使用されます)
	void		UpdateMinMax(const Vector3& point);
	// クォータニオンによって回転されました
	void		Rotate(const Quaternion& q);
	bool		Contains(const Vector3& point) const;
	float		MinDistSq(const Vector3& point) const;

	Vector3		GetBoxCenter() const;

	Vector3		sMin;
	Vector3		sMax;
};


struct OBB
{
	OBB(const Vector3& center, const Quaternion& rotation, const Vector3& extents);


	Vector3		sCenter;
	Vector3		sOffset;
	Quaternion	sRotation;
	Vector3		sExtents;
};

struct Capsule
{
				Capsule(const Vector3& start, const Vector3& end, float radius);
				Capsule(const LineSegment& segment, float radius);
	// 0 <= t <= 1 の間のセグメントに沿った点を取得する
	Vector3		PointOnSegment(float t) const;
	
	bool		Contains(const Vector3& point) const;

	float		SqrDistanceToSegment(const Vector3& point) const;

	LineSegment sSegment;
	float		sRadius;
};

struct ConvexPolygon
{
	bool				Contains(const Vector2& point) const;
	// 頂点は時計回りに並べられています
	vector<Vector2>		sVertices;
};

bool	OnCollision(const OBB& a, const OBB& b);
bool	OnCollision(const OBB& a, const Sphere& b);
bool	OnCollision(const OBB& a, const Capsule& b);
// 交差点機能
bool	OnCollision(const Sphere& a, const Sphere& b);
bool	OnCollision(const Sphere& a, const Capsule& b);

bool	OnCollision(const Capsule& capsule, const Sphere& b);

bool	OnCollision(const LineSegment& l, const Sphere& s, float& outT);
bool	OnCollision(const LineSegment& l, const Plane& p, float& outT);
bool	OnCollision(const LineSegment& l, const AABB& b, float& outT,Vector3& outNorm);
bool	OnRayAABBCollision(const LineSegment& seg, const AABB& box, float& outT, Vector3& outNorm);
bool	OnRayCastCollision(const LineSegment& rayWorld, OBB& obb, float& outT, Vector3& outNorm);


bool	OnCollision(const Capsule& a, const Capsule& b);

bool	SweptSphere(const Sphere& P0, const Sphere& P1,
		const Sphere& Q0, const Sphere& Q1, float& t);


void	ProjectOBB(const OBB& obb, const Vector3& axis, float& outMin, float& outMax);

float	ClosestDistanceSegmentToOBB(const LineSegment& seg, const OBB& obb);

Vector3 ClosestPointOnOBB(const Vector3& point, const OBB& obb);

void	ClosestPtsBetweenSegments(const LineSegment& s1, const LineSegment& s2, Vector3& outPt1, Vector3& outPt2);
