#include "Collision.h"

LineSegment::LineSegment(const Vector3& start, const Vector3& end)
	:mStart(start)
	, mEnd(end)
{
}

Vector3 LineSegment::PointOnSegment(float t) const
{
	float u = std::clamp(t, 0.0f, 1.0f);
	return mStart + (mEnd - mStart) * u;
}

float LineSegment::MinDistSq(const Vector3& point) const
{
	// Construct vectors
	Vector3 ab = mEnd - mStart;
	Vector3 ba = -1.0f * ab;
	Vector3 ac = point - mStart;
	Vector3 bc = point - mEnd;

	// Case 1: C projects prior to A
	if (Vector3::Dot(ab, ac) < 0.0f)
	{
		return ac.LengthSq();
	}
	// Case 2: C projects after B
	else if (Vector3::Dot(ba, bc) < 0.0f)
	{
		return bc.LengthSq();
	}
	// Case 3: C projects onto line
	else
	{
		// Compute p
		float scalar = Vector3::Dot(ac, ab)
			/ Vector3::Dot(ab, ab);
		Vector3 p = scalar * ab;
		// Compute length squared of ac - p
		return (ac - p).LengthSq();
	}
}

float LineSegment::MinDistSq(const LineSegment& s1, const LineSegment& s2)
{
	Vector3   u = s1.mEnd - s1.mStart;
	Vector3   v = s2.mEnd - s2.mStart;
	Vector3   w = s1.mStart - s2.mStart;
	float    a = Vector3::Dot(u, u);         // always >= 0
	float    b = Vector3::Dot(u, v);
	float    c = Vector3::Dot(v, v);         // always >= 0
	float    d = Vector3::Dot(u, w);
	float    e = Vector3::Dot(v, w);
	float    D = a * c - b * b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if (Math::NearZero(D)) // the lines are almost parallel
	{
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else // get the closest points on the infinite lines
	{                 
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0) // sc < 0 => the s=0 edge is visible
		{        
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) // sc > 1  => the s=1 edge is visible
		{  
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) // tc < 0 => the t=0 edge is visible
	{            
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else 
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) // tc > 1  => the t=1 edge is visible
	{      
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else 
		{
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (Math::NearZero(sN) ? 0.0f : sN / sD);
	tc = (Math::NearZero(tN) ? 0.0f : tN / tD);

	// get the difference of the two closest points
	Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return dP.LengthSq();   // return the closest distance squared
}


Plane::Plane(const Vector3& normal, float d)
	:mNormal(normal)
	, mD(d)
{
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
	// Compute vectors from a to b and a to c
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	// Cross product and normalize to get normal
	mNormal = Vector3::Cross(ab, ac);
	mNormal.Normalize();
	// d = -P dot n
	mD = -Vector3::Dot(a, mNormal);
}

float Plane::SignedDist(const Vector3& point) const
{
	return Vector3::Dot(point, mNormal) - mD;
}

Sphere::Sphere(const Vector3& center, float radius)
	:mCenter(center)
	, mRadius(radius)
{
}

bool Sphere::Contains(const Vector3& point) const
{
	// Get distance squared between center and point
	float distSq = (mCenter - point).LengthSq();
	return distSq <= (mRadius * mRadius);
}

AABB::AABB(const Vector3& min, const Vector3& max)
	: mMin(min)
	, mMax(max)
{
}

void AABB::UpdateMinMax(const Vector3& point)
{
	// Update each component separately
	mMin.x = Math::Min(mMin.x, point.x);
	mMin.y = Math::Min(mMin.y, point.y);
	mMin.z = Math::Min(mMin.z, point.z);

	mMax.x = Math::Max(mMax.x, point.x);
	mMax.y = Math::Max(mMax.y, point.y);
	mMax.z = Math::Max(mMax.z, point.z);
}

void AABB::Rotate(const Quaternion& q)
{
	// Construct the 8 points for the corners of the box
	std::array<Vector3, 8> points;
	// Min point is always a corner
	points[0] = mMin;
	// Permutations with 2 min and 1 max
	points[1] = Vector3(mMax.x, mMin.y, mMin.z);
	points[2] = Vector3(mMin.x, mMax.y, mMin.z);
	points[3] = Vector3(mMin.x, mMin.y, mMax.z);
	// Permutations with 2 max and 1 min
	points[4] = Vector3(mMin.x, mMax.y, mMax.z);
	points[5] = Vector3(mMax.x, mMin.y, mMax.z);
	points[6] = Vector3(mMax.x, mMax.y, mMin.z);
	// Max point corner
	points[7] = Vector3(mMax);

	// Rotate first point
	Vector3 p = Vector3::Transform(points[0], q);
	// Reset min/max to first point rotated
	mMin = p;
	mMax = p;
	// Update min/max based on remaining points, rotated
	for (size_t i = 1; i < points.size(); i++)
	{
		p = Vector3::Transform(points[i], q);
		UpdateMinMax(p);
	}
}

bool AABB::Contains(const Vector3& point) const
{
	bool outside = point.x < mMin.x ||
		point.y < mMin.y ||
		point.z < mMin.z ||
		point.x > mMax.x ||
		point.y > mMax.y ||
		point.z > mMax.z;
	// If none of these are true, the point is inside the box
	return !outside;
}

float AABB::MinDistSq(const Vector3& point) const
{
	// Compute differences for each axis
	float dx = Math::Max(mMin.x - point.x, 0.0f);
	dx = Math::Max(dx, point.x - mMax.x);
	float dy = Math::Max(mMin.y - point.y, 0.0f);
	dy = Math::Max(dy, point.y - mMax.y);
	float dz = Math::Max(mMin.z - point.z, 0.0f);
	dz = Math::Max(dy, point.z - mMax.z);
	// Distance squared formula
	return dx * dx + dy * dy + dz * dz;
}

Vector3 AABB::GetBoxCenter() const
{
	return (mMin + mMax) * 0.5f;
}

Vector3 ClosestPtSegmentAABB(const Vector3& p1, const Vector3& p2, const AABB& box)
{
	// 線分上の最近接点をAABBにクランプ
	Vector3 closest;

	// 線分からAABB上の最短点を探す
	Vector3 ab = p2 - p1;
	float t = 0.0f;

	// 最近接点を線分上で補間して使う
	float abLenSq = ab.LengthSq();
	if (abLenSq > 0.0f)
	{
		// クランプされた最近接点
		t = Vector3::Dot(box.GetBoxCenter() - p1,ab) / abLenSq;
		t = Math::Clamp(t, 0.0f, 1.0f);
	}

	Vector3 pointOnSeg = p1 + ab * t;

	// AABBにクランプ
	closest.x = Math::Clamp(pointOnSeg.x, box.mMin.x, box.mMax.x);
	closest.y = Math::Clamp(pointOnSeg.y, box.mMin.y, box.mMax.y);
	closest.z = Math::Clamp(pointOnSeg.z, box.mMin.z, box.mMax.z);

	return closest;
}

Capsule::Capsule(const Vector3& start, const Vector3& end, float radius)
	:mSegment(start, end)
	, mRadius(radius)
{
}

Vector3 Capsule::PointOnSegment(float t) const
{
	return mSegment.PointOnSegment(t);
}

bool Capsule::Contains(const Vector3& point) const
{
	// Get minimal dist. sq. between point and line segment
	float distSq = mSegment.MinDistSq(point);
	return distSq <= (mRadius * mRadius);
}

float Capsule::SqrDistanceToSegment(const Vector3& point) const
{
	Vector3 ab = mSegment.mEnd - mSegment.mStart;
	Vector3 ap = point - mSegment.mStart;

	float t = Vector3::Dot(ap, ab) / ab.LengthSq();
	t = Math::Clamp(t, 0.0f, 1.0f);

	Vector3 closest = mSegment.mStart + ab * t;
	return (point - closest).LengthSq();
}

bool ConvexPolygon::Contains(const Vector2& point) const
{
	float sum = 0.0f;
	Vector2 a, b;
	for (size_t i = 0; i < mVertices.size() - 1; i++)
	{
		// From point to first vertex
		a = mVertices[i] - point;
		a.Normalize();
		// From point to second vertex
		b = mVertices[i + 1] - point;
		b.Normalize();
		// Add angle to sum
		sum += Math::Acos(Vector2::Dot(a, b));
	}
	// Have to add angle for last vertex and first vertex
	a = mVertices.back() - point;
	a.Normalize();
	b = mVertices.front() - point;
	b.Normalize();
	sum += Math::Acos(Vector2::Dot(a, b));
	// Return true if approximately 2pi
	return Math::NearZero(sum - Math::TwoPi);
}
bool OnCollision(const OBB& a, const OBB& b)
{
	Vector3 aAxes[3] = {
	Vector3::Transform(Vector3::UnitX, a.mRotation),
	Vector3::Transform(Vector3::UnitY, a.mRotation),
	Vector3::Transform(Vector3::UnitZ, a.mRotation)
	};

	Vector3 bAxes[3] = {
		Vector3::Transform(Vector3::UnitX, b.mRotation),
		Vector3::Transform(Vector3::UnitY, b.mRotation),
		Vector3::Transform(Vector3::UnitZ, b.mRotation)
	};

	Vector3 axes[15];
	int axisCount = 0;

	for (int i = 0; i < 3; ++i) axes[axisCount++] = aAxes[i];
	for (int i = 0; i < 3; ++i) axes[axisCount++] = bAxes[i];
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Vector3 cross = Vector3::Cross(aAxes[i], bAxes[j]);
			if (cross.LengthSq() > 1e-6f)
				axes[axisCount++] = cross.Normalized();
		}
	}

	for (int i = 0; i < axisCount; ++i)
	{
		Vector3 axis = axes[i];

		float minA, maxA, minB, maxB;
		ProjectOBB(a, axis, minA, maxA);
		ProjectOBB(b, axis, minB, maxB);

		if (std::min(maxA, maxB) - std::max(minA, minB) < 0.0f)
		{
			// 分離軸が見つかった → 非衝突
			return false;
		}
	}

	// すべての軸で重なりがある → 衝突している
	return true;
}
bool OnCollision(const OBB& a, const Sphere& b)
{
	// 1. Sphere中心をOBBのローカル座標に変換
	Vector3 localSphereCenter = b.mCenter - a.mCenter;

	// OBBの3つの軸（基底ベクトル）
	Vector3 axes[3] = {
		Vector3::Transform(Vector3::UnitX, a.mRotation),
		Vector3::Transform(Vector3::UnitY, a.mRotation),
		Vector3::Transform(Vector3::UnitZ, a.mRotation)
	};

	// Sphere中心のローカル座標
	Vector3 localPoint(0, 0, 0);

	// 2. 各軸に投影し、Extentsの範囲にクランプ
	for (int i = 0; i < 3; ++i)
	{
		float dist = Vector3::Dot(localSphereCenter, axes[i]);
		float extent = (i == 0) ? a.mExtents.x : (i == 1) ? a.mExtents.y : a.mExtents.z;
		float clampedDist = std::max(-extent, std::min(dist, extent));
		localPoint += axes[i] * clampedDist;
	}

	// 3. Sphere中心と最近点の距離を計算
	Vector3 difference = localPoint - localSphereCenter;
	float distanceSq = difference.LengthSq();

	// 半径の二乗と比較
	return distanceSq <= b.mRadius * b.mRadius;
}
bool OnCollision(const OBB& a, const Capsule& b)
{
	float dist = ClosestDistanceSegmentToOBB(b.mSegment, a);
	return dist <= b.mRadius;
}
//球同士の当たり判定
bool OnCollision(const Sphere& a, const Sphere& b)
{
	float distSq = (a.mCenter - b.mCenter).LengthSq();
	float sumRadii = a.mRadius + b.mRadius;
	return distSq <= (sumRadii * sumRadii);
}

//カプセル同士の当たり判定
bool OnCollision(const Capsule& a, const Capsule& b)
{
	float distSq = LineSegment::MinDistSq(a.mSegment,
		b.mSegment);
	float sumRadii = a.mRadius + b.mRadius;
	return distSq <= (sumRadii * sumRadii);
}

//カプセルと球の当たり判定
bool OnCollision(const Capsule& a, const Sphere& b)
{
	// カプセルの線分
	Vector3 segStart = a.mSegment.mStart;
	Vector3 segEnd = a.mSegment.mEnd;

	// 球の中心
	Vector3 center = b.mCenter;

	// 線分と球中心の最近接点
	Vector3 ab = segEnd - segStart;
	Vector3 ac = center - segStart;

	float abLenSq = ab.LengthSq();
	float t = Vector3::Dot(ac, ab) / abLenSq;
	t = Math::Clamp(t, 0.0f, 1.0f);

	Vector3 closest = segStart + ab * t;

	// 距離²を計算
	float sqDist = (center - closest).LengthSq();

	// 合計半径の距離²と比較
	float radiusSum = a.mRadius + b.mRadius;
	return sqDist <= radiusSum * radiusSum;
}
//球とカプセルの当たり判定
bool OnCollision(const Sphere& a, const Capsule& b)
{
	float distSq = b.mSegment.MinDistSq(a.mCenter);
	float radiusSum = a.mRadius + b.mRadius;
	return distSq <= radiusSum * radiusSum;
}

//線分と球の当たり判定
bool OnCollision(const LineSegment& l, const Sphere& s, float& outT)
{
	// Compute X, Y, a, b, c as per equations
	Vector3 X = l.mStart - s.mCenter;
	Vector3 Y = l.mEnd - l.mStart;
	float a = Vector3::Dot(Y, Y);
	float b = 2.0f * Vector3::Dot(X, Y);
	float c = Vector3::Dot(X, X) - s.mRadius * s.mRadius;
	// Compute discriminant
	float disc = b * b - 4.0f * a * c;
	if (disc < 0.0f)
	{
		return false;
	}
	else
	{
		disc = Math::Sqrt(disc);
		// Compute min and max solutions of t
		float tMin = (-b - disc) / (2.0f * a);
		float tMax = (-b + disc) / (2.0f * a);
		// Check whether either t is within bounds of segment
		if (tMin >= 0.0f && tMin <= 1.0f)
		{
			outT = tMin;
			return true;
		}
		else if (tMax >= 0.0f && tMax <= 1.0f)
		{
			outT = tMax;
			return true;
		}
		else
		{
			return false;
		}
	}
}
//線分と平面の当たり判定
bool OnCollision(const LineSegment& l, const Plane& p, float& outT)
{
	// First test if there's a solution for t
	float denom = Vector3::Dot(l.mEnd - l.mStart,
		p.mNormal);
	if (Math::NearZero(denom))
	{
		// The only way they intersect is if start
		// is a point on the plane (P dot N) == d
		if (Math::NearZero(Vector3::Dot(l.mStart, p.mNormal)
			- p.mD))
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
		float numer = -Vector3::Dot(l.mStart, p.mNormal) -
			p.mD;
		outT = numer / denom;
		// Validate t is within bounds of the line segment
		if (outT >= 0.0f && outT <= 1.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
//線分とボックスの当たり判定で使う関数
bool TestSidePlane(float start, float end, float negd, const Vector3& norm,
	vector<std::pair<float, Vector3>>& out)
{
	float denom = end - start;
	if (Math::NearZero(denom))
	{
		return false;
	}
	else
	{
		float numer = -start + negd;
		float t = numer / denom;
		// Test that t is within bounds
		if (t >= 0.0f && t <= 1.0f)
		{
			out.emplace_back(t, norm);
			return true;
		}
		else
		{
			return false;
		}
	}
}
//線分とボックスの当たり判定
bool OnCollision(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm)
{
	// Vector to save all possible t values, and normals for those sides
	vector<std::pair<float, Vector3>> tValues;
	// Test the x planes
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMin.x, Vector3::NegUnitX,tValues);
	//TestSidePlane(l.mStart.x, l.mEnd.x, b.mMax.x, Vector3::UnitX,tValues);
	// Test the y planes
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMin.y, Vector3::NegUnitY,tValues);
	//TestSidePlane(l.mStart.y, l.mEnd.y, b.mMax.y, Vector3::UnitY,tValues);
	// Test the z planes
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMin.z, Vector3::NegUnitZ,tValues);
	//TestSidePlane(l.mStart.z, l.mEnd.z, b.mMax.z, Vector3::UnitZ,tValues);

	// Sort the t values in ascending order
	std::sort(tValues.begin(), tValues.end(), [](
		const std::pair<float, Vector3>& a,
		const std::pair<float, Vector3>& b) {
			return a.first < b.first;
		});
	// Test if the box contains any of these points of intersection
	Vector3 point;
	for (auto& t : tValues)
	{
		point = l.PointOnSegment(t.first);
		if (b.Contains(point))
		{
			outT = t.first;
			outNorm = t.second;
			return true;
		}
	}

	//None of the intersections are within bounds of box
	return false;
}

bool OnRayAABBCollision(const LineSegment& seg, const AABB& box, float& outT, Vector3& outNorm)
{
	Vector3 dir = seg.mEnd - seg.mStart;

	float tMin = 0.0f;
	float tMax = 1.0f;
	Vector3 hitNormal = Vector3::Zero;

	auto checkAxis = [&](float start, float d, float minB, float maxB, const Vector3& axisMinNormal) -> bool
		{
			if (Math::NearZero(d))
			{
				// 平行 → スラブ外なら交差なし
				if (start < minB || start > maxB)
					return false;
			}
			else
			{
				float ood = 1.0f / d;
				float t1 = (minB - start) * ood;
				float t2 = (maxB - start) * ood;

				Vector3 axisNormal = axisMinNormal;

				if (t1 > t2)
				{
					std::swap(t1, t2);
					axisNormal = -1 * axisNormal; // max面が最初に当たる
				}

				if (t1 > tMin)
				{
					tMin = t1;
					hitNormal = axisNormal;
				}
				tMax = Math::Min(tMax, t2);

				if (tMin > tMax)
					return false;
			}
			return true;
		};

	// X軸
	if (!checkAxis(seg.mStart.x, dir.x, box.mMin.x, box.mMax.x, Vector3::NegUnitX))
		return false;
	// Y軸
	if (!checkAxis(seg.mStart.y, dir.y, box.mMin.y, box.mMax.y, Vector3::NegUnitY))
		return false;
	// Z軸
	if (!checkAxis(seg.mStart.z, dir.z, box.mMin.z, box.mMax.z, Vector3::NegUnitZ))
		return false;

	if (tMin >= 0.0f && tMin <= 1.0f)
	{
		outT = tMin;
		outNorm = hitNormal;
		return true;
	}
	return false;
}

bool OnRayCastCollision(const LineSegment& rayWorld, OBB& obb, float& outT, Vector3& outNorm)
{
	Quaternion invRot = obb.mRotation;
	invRot.Conjugate(); // 逆回転を取得
	Vector3 localStart = Vector3::Transform(rayWorld.mStart - obb.mCenter, invRot);
	Vector3 localEnd = Vector3::Transform(rayWorld.mEnd - obb.mCenter, invRot);
	LineSegment localRay(localStart, localEnd);

	AABB localBox(-1.0f * obb.mExtents, obb.mExtents);

	float localT;
	Vector3 localNormal;

	if (OnRayAABBCollision(localRay, localBox, localT, localNormal))
	{
		outT = localT;
		outNorm = Vector3::Transform(localNormal, obb.mRotation);
		outNorm.Normalize();
		return true;
	}
	return false;
}

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& outT)
{
	// Compute X, Y, a, b, and c
	Vector3 X = P0.mCenter - Q0.mCenter;
	Vector3 Y = P1.mCenter - P0.mCenter -
		(Q1.mCenter - Q0.mCenter);
	float a = Vector3::Dot(Y, Y);
	float b = 2.0f * Vector3::Dot(X, Y);
	float sumRadii = P0.mRadius + Q0.mRadius;
	float c = Vector3::Dot(X, X) - sumRadii * sumRadii;
	// Solve discriminant
	float disc = b * b - 4.0f * a * c;
	if (disc < 0.0f)
	{
		return false;
	}
	else
	{
		disc = Math::Sqrt(disc);
		// We only care about the smaller solution
		outT = (-b - disc) / (2.0f * a);
		if (outT >= 0.0f && outT <= 0.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void ProjectOBB(const OBB& obb, const Vector3& axis, float& outMin, float& outMax)
{
	Vector3 x = Vector3::Transform(Vector3::UnitX, obb.mRotation) * obb.mExtents.x;
	Vector3 y = Vector3::Transform(Vector3::UnitY, obb.mRotation) * obb.mExtents.y;
	Vector3 z = Vector3::Transform(Vector3::UnitZ, obb.mRotation) * obb.mExtents.z;

	Vector3 corners[8] = {
		obb.mCenter + x + y + z,
		obb.mCenter + x + y - z,
		obb.mCenter + x - y + z,
		obb.mCenter + x - y - z,
		obb.mCenter - x + y + z,
		obb.mCenter - x + y - z,
		obb.mCenter - x - y + z,
		obb.mCenter - x - y - z,
	};

	outMin = outMax = Vector3::Dot(axis, corners[0]);
	for (int i = 1; i < 8; ++i)
	{
		float proj = Vector3::Dot(axis, corners[i]);
		outMin = std::min(outMin, proj);
		outMax = std::max(outMax, proj);
	}
}

float ClosestDistanceSegmentToOBB(const LineSegment& seg, const OBB& obb)
{
	// t ∈ [0,1] を細かく分割して調査（簡易）
	const int steps = 10;
	float minDistSq = Math::Infinity;

	for (int i = 0; i <= steps; ++i)
	{
		float t = i / static_cast<float>(steps);
		Vector3 pointOnSegment = seg.mStart + (seg.mEnd - seg.mStart) * t;
		Vector3 closestOnOBB = ClosestPointOnOBB(pointOnSegment, obb);
		float distSq = (pointOnSegment - closestOnOBB).LengthSq();
		if (distSq < minDistSq)
		{
			minDistSq = distSq;
		}
	}

	return std::sqrt(minDistSq); // 距離を返す
}

Vector3 ClosestPointOnOBB(const Vector3& point, const OBB& obb)
{
	Vector3 d = point - obb.mCenter;
	Vector3 closest = obb.mCenter;

	for (int i = 0; i < 3; ++i)
	{
		Vector3 axis = Vector3::Transform(Vector3::Axis(i), obb.mRotation);
		float dist = Vector3::Dot(d, axis);
		float extent = (i == 0) ? obb.mExtents.x : (i == 1) ? obb.mExtents.y : obb.mExtents.z;
		dist = Math::Clamp(dist, -extent, extent);
		closest += axis * dist;
	}

	return closest;
}

void ClosestPtsBetweenSegments(const LineSegment& s1, const LineSegment& s2, Vector3& outPt1, Vector3& outPt2)
{
	Vector3 d1 = s1.mStart - s1.mEnd; // 線分1の方向ベクトル
	Vector3 d2 = s2.mStart - s2.mEnd; // 線分2の方向ベクトル
	Vector3 r = s1.mStart - s2.mStart;

	float a = Vector3::Dot(d1, d1); // |d1|^2
	float e = Vector3::Dot(d2, d2); // |d2|^2
	float f = Vector3::Dot(d2, r);

	float s, t;

	const float EPSILON = 1e-6f;

	if (a <= EPSILON && e <= EPSILON)
	{
		// 両方とも点扱い
		outPt1 = s1.mStart;
		outPt2 = s2.mStart;
		return;
	}

	if (a <= EPSILON)
	{
		// 線分1が点
		s = 0.0f;
		t = Math::Clamp(f / e, 0.0f, 1.0f);
	}
	else
	{
		float c = Vector3::Dot(d1, r);
		if (e <= EPSILON)
		{
			// 線分2が点
			t = 0.0f;
			s = Math::Clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			float b = Vector3::Dot(d1, d2);
			float denom = a * e - b * b;

			if (denom != 0.0f)
			{
				s = Math::Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f;
			}

			t = (b * s + f) / e;

			if (t < 0.0f)
			{
				t = 0.0f;
				s = Math::Clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = Math::Clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	outPt1 = s1.mStart + d1 * s;
	outPt2 = s2.mStart + d2 * t;
}

OBB::OBB(const Vector3& center, const Quaternion& rotation, const Vector3& extents)
	: mCenter(center)
	, mRotation(rotation)
	, mExtents(extents)
	, mOffset(Vector3::Zero)
{
}
