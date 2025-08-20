#include "ColliderActor.h"


ColliderActor::ColliderActor()
	: Transform()
	, mRigidbody(nullptr)
	, mCollider(nullptr)
{
}

ColliderActor::~ColliderActor()
{
}
