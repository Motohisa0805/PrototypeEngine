#pragma once
#include "StandardLibrary.h"
#include "Math.h"
#include "Collision.h"
#include "Rigidbody.h"
#include "Physics.h"

// 衝突の接触点をまとめた構造体
struct ContactManifold {
	Rigidbody*		gRbA;
	Rigidbody*		gRbB;
	Vector3			gNormal;
	float			gPenetration;
	vector<Vector3> gContactPoints;
	bool 			gProcessed;
};

class PhysWorld;

//PhysWorldのIsLandPhysicクラス
class IsLandPhysic
{
private:
	PhysWorld*			mWorld;

	vector<Rigidbody*>	mActiveBodies;
public:
	IsLandPhysic(PhysWorld* world);

	void BuildAndSolveIslands(vector<ContactManifold>& manifolds, float deltaTime);

	void SolveIsland(vector<Rigidbody*>& islandBodies, vector<ContactManifold*>& manifolds, float deltaTime);


	void AddActiveBodies(Rigidbody* rb);

	void RemoveActioveBodies(Rigidbody* rb);
};

