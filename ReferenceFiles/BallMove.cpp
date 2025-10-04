#include "BallMove.h"
#include "Actor.h"
#include "BaseScene.h"
#include "PhysWorld.h"
#include "BallActor.h"

BallMove::BallMove(ActorObject* owner)
	:MoveComponent(owner)
{
}

void BallMove::Update(float deltaTime)
{
	// 移動方向に沿ってセグメントを構築する
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetLocalPosition();
	Vector3 dir = mOwner->GetForward();
	Vector3 end = start + dir * segmentLength;

	/*
	// 線分を作成する
	LineSegment l(start, end);
	//TODO : 線分とボックス当たり判定
	// テストセグメント対世界
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	// (プレイヤーと衝突しない)
	if (phys->RayCast(l, info) && info.mActor != mPlayer)
	{
		// もし衝突したら、法線に対してボールを反射
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);
		// ターゲットに衝突したかどうか
		TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
		if (target)
		{
			static_cast<BallActor*>(mOwner)->HitTarget();
		}
	}
	*/

	// 基底クラスのアップデートは前方の速度に基づいて移動します
	MoveComponent::Update(deltaTime);
}
