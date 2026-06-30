#include "PhysWorld.h"
#include "Actor.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "Time.h"
#include <SDL3/SDL.h>

PhysWorld::PhysWorld() : mLandPhysic(nullptr)
{
    if (!mLandPhysic)
    {
        mLandPhysic = new IsLandPhysic(this);
    }
}

PhysWorld::~PhysWorld()
{
    if (mLandPhysic)
    {
        delete mLandPhysic;
        mLandPhysic = nullptr;
    }
}

bool PhysWorld::RayCast(const LineSegment& l, CollisionInfo& outColl, int tag)
{
    bool    collided = false;
    float   closestT = Math::Infinity;
    Vector3 norm     = Vector3::Zero;

    for (auto collider : mCollider)
    {
        if (tag != -1 && (int)collider->GetOwner()->GetActorTag() != tag)
            continue;

        OBB   obb = collider->GetWorldOBB();
        float t   = 0;
        if (OnRayCastCollision(l, obb, t, norm))
        {
            if (t >= 0.0f && t < closestT)
            {
                closestT          = t;
                outColl.mPoint    = l.PointOnSegment(t);
                outColl.mNormal   = norm;
                outColl.mCollider = collider;
                outColl.mActor    = collider->GetActor();
                collided          = true;
            }
        }
    }
    return collided;
}

std::vector<PhysWorld::CollisionInfo>
PhysWorld::RayCastAll(const LineSegment& l)
{
    std::vector<CollisionInfo> results;

    for (auto collider : mCollider)
    {
        float   t    = 0.0f;
        Vector3 norm = Vector3::Zero;
        // OBBを持つBoxColliderに対しては、RayCast(LineSegment, OBB)で判定する
        if (collider->GetColliderType() == Collider::BoxType)
        {
            OBB obb = collider->GetWorldOBB();
            if (OnRayCastCollision(l, obb, t, norm))
            {
                if (t >= 0.0f && t <= 1.0f)
                {
                    CollisionInfo info;
                    info.mT        = t;
                    info.mPoint    = l.PointOnSegment(t);
                    info.mNormal   = norm;
                    info.mCollider = collider;
                    info.mActor    = collider->GetActor();
                    results.push_back(info);
                }
            }
            // TODO : のちのち他コライダーも追加
            /*
            if (OnCollision(l, collider->GetWorldAABBFromOBB(), t, norm))
            {
                if (t >= 0.0f && t <= 1.0f)
                {
                    CollisionInfo info;
                    info.mT = t;
                    info.mPoint = l.PointOnSegment(t);
                    info.mNormal = norm;
                    info.mCollider = collider;
                    info.mActor = collider->GetOwner();
                    results.push_back(info);
                }
            }
            */
        }
    }

    // t値（手前順）でソートする
    std::sort(results.begin(), results.end(),
              [](const CollisionInfo& a, const CollisionInfo& b)
              { return a.mT < b.mT; });

    return results;
}

void PhysWorld::SweepAndPruneXYZ(float deltaTime)
{
    // X軸でソート
    std::sort(mColliderXAxis.begin(), mColliderXAxis.end(),
              [](Collider* a, Collider* b)
              {
                  return a->GetWorldAABBFromOBB().mMin.x <
                         b->GetWorldAABBFromOBB().mMin.x;
              });

    mCurrentHitPairs.clear();

    vector<ContactManifold> manifolds;

    // X軸スイープ開始
    for (size_t i = 0; i < mColliderXAxis.size(); ++i)
    {
        Collider*   colliderA = mColliderXAxis[i];
        const AABB& aabbA     = colliderA->GetWorldAABBFromOBB();

        for (size_t j = i + 1; j < mColliderXAxis.size(); ++j)
        {
            Collider*   colliderB = mColliderXAxis[j];
            const AABB& aabbB     = colliderB->GetWorldAABBFromOBB();

            const float contactOffsetA = colliderA->GetContactOffset();
            const float contactOffsetB = colliderB->GetContactOffset();

            // X軸の最大と最小が交差してなかったらbreak（高速化）
            // ※ contactOffset を加味して比較
            if (aabbB.mMin.x - contactOffsetB > aabbA.mMax.x + contactOffsetA)
            {
                break;
            }

            // Y軸とZ軸の交差判定にも contactOffset を考慮
            if (aabbA.mMax.y + contactOffsetA < aabbB.mMin.y - contactOffsetB ||
                aabbA.mMin.y - contactOffsetA > aabbB.mMax.y + contactOffsetB)
            {
                continue;
            }

            if (aabbA.mMax.z + contactOffsetA < aabbB.mMin.z - contactOffsetB ||
                aabbA.mMin.z - contactOffsetA > aabbB.mMax.z + contactOffsetB)
            {
                continue;
            }

            // ここまで来たらAとBはAABB上は当たっている（各コライダーで精密判定を入れる）
            if (!IsOnCollision(colliderA, colliderB))
            {
                continue; // SAT上は衝突していないのでスキップ
            }

            // ここまで来たらAとBは当たっている
            auto actorA = colliderA->GetActor();
            auto actorB = colliderB->GetActor();

            std::pair<ActorObject*, ActorObject*> sortedPair =
                actorA < actorB ? std::make_pair(actorA, actorB)
                                : std::make_pair(actorB, actorA);

            mCurrentHitPairs.emplace(sortedPair);

            // Enter or Stay判定
            if (mPrevHitPairs.count(sortedPair))
            {
                actorA->OnCollisionStay(actorB);
                actorB->OnCollisionStay(actorA);
            }
            else
            {
                actorA->OnCollisionEnter(actorB);
                actorB->OnCollisionEnter(actorA);
            }

            // 押し出し処理のためのContactManifoldを作成する
            if (colliderA->IsCollider() && colliderB->IsCollider())
            {
                if (colliderA->IsCollider() && colliderB->IsCollider())
                {
                    Rigidbody* rbA = actorA->GetComponent<Rigidbody>();
                    Rigidbody* rbB = actorB->GetComponent<Rigidbody>();

                    // 両方ともRigidbodyがない（静止物同士）なら計算不要
                    if (!rbA && !rbB)
                        continue;
                    // 両方ともRigidbodyがあって、両方ともスリープ状態なら計算不要
                    if (rbA && rbB && rbA->IsSleeping() && rbB->IsSleeping())
                        continue;
                    std::vector<ContactPoint> cpList;
                    if (IsCollectContactPoints(colliderA, colliderB, cpList,
                                               contactOffsetA + contactOffsetB))
                    {
                        ContactManifold m;
                        m.gProcessed = false;
                        m.gRbA       = rbA;
                        m.gRbB       = rbB;
                        m.gNormal    = cpList[0].mNormal;

                        m.gPenetration = cpList[0].mPenetration;
                        for (auto& cp : cpList)
                            m.gContactPoints.push_back(cp.mPosition);
                        // スリープ状態を解除する（どちらかがスリープ状態なら両方とも起こす）
                        if (rbA)
                        {
                            if (rbA->IsSleeping())
                            {
                                rbA->WakeUp();
                            }
                            mLandPhysic->AddActiveBodies(
                                rbA); // スリープから起こしたRigidbodyをIsLandPhysicのアクティブリストに追加
                        }
                        if (rbB)
                        {
                            if (rbB->IsSleeping())
                            {
                                rbB->WakeUp();
                            }
                            mLandPhysic->AddActiveBodies(
                                rbB); // スリープから起こしたRigidbodyをIsLandPhysicのアクティブリストに追加
                        }

                        manifolds.push_back(m);
                    }
                }
            }
        }
    }

    // 島の概念の処理
    mLandPhysic->BuildAndSolveIslands(manifolds, deltaTime);
    // 集めた全ペアに対して、反復計算（ソルバー）を実行する
    // ここで初めて物体が動かされる。1フレームに1回だけこの関数を呼ぶ。
    // ApplyIterations(manifolds, deltaTime);

    // Exitチェック
    for (const auto& pair : mPrevHitPairs)
    {
        if (mCurrentHitPairs.count(pair) == 0)
        {
            auto actorA = pair.first;
            auto actorB = pair.second;
            actorA->OnCollisionExit(actorB);
            actorB->OnCollisionExit(actorA);
        }
    }

    // 状態更新
    mPrevHitPairs = mCurrentHitPairs;
}

bool PhysWorld::IsOnCollision(Collider* colliderA, Collider* colliderB)
{
    if (colliderA->GetColliderType() == Collider::BoxType &&
        colliderB->GetColliderType() == Collider::BoxType)
    {
        return OnCollision(colliderA->GetWorldOBB(), colliderB->GetWorldOBB());
    }
    else if (colliderA->GetColliderType() == Collider::BoxType &&
             colliderB->GetColliderType() == Collider::SphereType)
    {
        return OnCollision(colliderA->GetWorldOBB(),
                           colliderB->GetWorldSphere());
    }
    else if (colliderA->GetColliderType() == Collider::SphereType &&
             colliderB->GetColliderType() == Collider::BoxType)
    {
        return OnCollision(colliderB->GetWorldOBB(),
                           colliderA->GetWorldSphere());
    }
    else if (colliderA->GetColliderType() == Collider::BoxType &&
             colliderB->GetColliderType() == Collider::CapsuleType)
    {
        return OnCollision(colliderA->GetWorldOBB(),
                           colliderB->GetWorldCapsule());
    }
    else if (colliderA->GetColliderType() == Collider::CapsuleType &&
             colliderB->GetColliderType() == Collider::BoxType)
    {
        return OnCollision(colliderB->GetWorldOBB(),
                           colliderA->GetWorldCapsule());
    }

    if (colliderA->GetColliderType() == Collider::SphereType &&
        colliderB->GetColliderType() == Collider::SphereType)
    {
        return OnCollision(colliderA->GetWorldSphere(),
                           colliderB->GetWorldSphere());
    }
    else if (colliderA->GetColliderType() == Collider::SphereType &&
             colliderB->GetColliderType() == Collider::CapsuleType)
    {
        return OnCollision(colliderA->GetWorldSphere(),
                           colliderB->GetWorldCapsule());
    }
    else if (colliderA->GetColliderType() == Collider::CapsuleType &&
             colliderB->GetColliderType() == Collider::SphereType)
    {
        return OnCollision(colliderA->GetWorldCapsule(),
                           colliderB->GetWorldSphere());
    }

    if (colliderA->GetColliderType() == Collider::CapsuleType &&
        colliderB->GetColliderType() == Collider::CapsuleType)
    {
        return OnCollision(colliderA->GetWorldCapsule(),
                           colliderB->GetWorldCapsule());
    }

    return false;
}

bool PhysWorld::IsCollectContactPoints(Collider* colliderA, Collider* colliderB,
                                       std::vector<ContactPoint>& outContacts,
                                       float                      contactOffset)
{
    Collider* cA      = colliderA;
    Collider* cB      = colliderB;
    bool      swapped = false;

    // 型の順序を強制する (例: Box < Sphere < Capsule)
    // こうすることで (Sphere, Box) という組み合わせを (Box, Sphere)
    // として扱える
    if (cA->GetColliderType() > cB->GetColliderType())
    {
        std::swap(cA, cB);
        swapped = true;
    }

    bool                   result = false;
    Collider::ColliderType typeA  = cA->GetColliderType();
    Collider::ColliderType typeB  = cB->GetColliderType();

    // Colliderの型の組み合わせに応じて、適切な接触点収集関数を呼び出す
    if (typeA == Collider::BoxType)
    {
        if (typeB == Collider::BoxType)
            result = CollectContactPoints_OBB_OBB(cA->GetWorldOBB(),
                                                  cB->GetWorldOBB(),
                                                  outContacts, contactOffset);
        else if (typeB == Collider::SphereType)
            result = CollectContactPoints_OBB_Sphere(
                cA->GetWorldOBB(), cB->GetWorldSphere(), outContacts,
                contactOffset);
        else if (typeB == Collider::CapsuleType)
            result = CollectContactPoints_OBB_Capsule(
                cA->GetWorldOBB(), cB->GetWorldCapsule(), outContacts,
                contactOffset);
    }
    else if (typeA == Collider::SphereType)
    {
        if (typeB == Collider::SphereType)
            result = CollectContactPoints_Sphere_Sphere(
                cA->GetWorldSphere(), cB->GetWorldSphere(), outContacts,
                contactOffset);
        else if (typeB == Collider::CapsuleType)
            result = CollectContactPoints_Sphere_Capsule(
                cA->GetWorldSphere(), cB->GetWorldCapsule(), outContacts,
                contactOffset);
    }
    else if (typeA == Collider::CapsuleType)
    {
        if (typeB == Collider::CapsuleType)
            result = CollectContactPoints_Capsule_Capsule(
                cA->GetWorldCapsule(), cB->GetWorldCapsule(), outContacts,
                contactOffset);
    }

    if (result && swapped)
    {
        for (auto& cp : outContacts)
        {
            cp.mNormal = -1.0f * cp.mNormal;
        }
    }
    return result;
}

void PhysWorld::ApplyIterations(std::vector<ContactManifold>& manifolds,
                                float                         deltaTime)
{
    const int velocityIterations =
        Physics::VELOCITY_ITERATIONS; // 速度（跳ね返り・摩擦）の反復回数
    const int positionIterations =
        Physics::POSITION_ITERATIONS; // 位置（めり込み押し出し）の反復回数

    // 1. 速度のイテレーション（これを繰り返すとジェンガが安定する）
    for (int i = 0; i < velocityIterations; ++i)
    {
        for (auto& m : manifolds)
        {
            for (auto& point : m.gContactPoints)
            {
                // Rigidbodyの速度と角速度だけを更新する
                // ResolveCollisionの中から「座標更新」を抜いた処理を呼ぶ
                if (m.gRbA)
                {
                    // Aが動的オブジェクトの場合
                    m.gRbA->ResolveVelocity(m.gRbB, m.gNormal, point,
                                            deltaTime);
                }
                else if (m.gRbB)
                {
                    // Aが静止物で、Bだけが動的オブジェクトの場合
                    // Bから見て「A（静止物）」とぶつかった計算にするため、法線を反転して呼ぶ
                    m.gRbB->ResolveVelocity(nullptr, m.gNormal, point,
                                            deltaTime);
                }
            }
        }
    }
    // 2. 位置のイテレーション（めり込みを解消する）
    ResolvePositions(manifolds, positionIterations);
}

void PhysWorld::OneResolvePosition(ContactManifold& m)
{

    // Rigidbodyの取得（付いていない場合は nullptr）
    Rigidbody* rbA = m.gRbA;
    Rigidbody* rbB = m.gRbB;

    // 質量の逆数を取得。Rigidbodyがない（静的）場合は 0.0f と見なす
    float invMassA = (rbA != nullptr) ? rbA->GetInverseMass() : 0.0f;
    float invMassB = (rbB != nullptr) ? rbB->GetInverseMass() : 0.0f;

    // 両方の InverseMass の合計
    float sumInvMass = invMassA + invMassB;

    // どちらも動かない物体（合計がゼロ）なら何もしない
    if (sumInvMass <= 0.0001f)
    {
        return;
    }

    // 押し出しの割合（AとBでどう分担するか）
    float ratioA = invMassA / sumInvMass;
    float ratioB = invMassB / sumInvMass;

    // めり込み量に対して、少し余裕（0.01fなど）を残して押し出す（ジッター対策）
    const float slop = 0.01f;
    const float percent =
        0.8f; // 1.0にすると跳ねすぎることがあるので80%程度にする
    float correctionMagnitude = std::max(m.gPenetration - slop, 0.0f) * percent;

    // 各オブジェクトの押し出しベクトルを計算（重いほど動かない）
    Vector3 correctionA =
        m.gNormal * (correctionMagnitude * (invMassA / sumInvMass));
    Vector3 correctionB =
        m.gNormal * (correctionMagnitude * (invMassB / sumInvMass));

    if (m.gRbA)
    {
        // Aを法線方向に移動
        Vector3 posA = m.gRbA->GetActor()->GetTransform()->GetLocalPosition();
        m.gRbA->GetActor()->GetTransform()->SetLocalPosition(posA -
                                                             correctionA);
        m.gRbA->GetActor()->GetTransform()->ComputeWorldTransform();
    }

    if (m.gRbB)
    {
        // Bを法線の【逆】方向に移動
        Vector3 posB = m.gRbB->GetActor()->GetTransform()->GetLocalPosition();
        m.gRbB->GetActor()->GetTransform()->SetLocalPosition(posB +
                                                             correctionB);
        m.gRbB->GetActor()->GetTransform()->ComputeWorldTransform();
    }
    // 最終的なめり込み量を更新（残った分だけ次のイテレーションで解消する）
    m.gPenetration -= correctionMagnitude;
}

void PhysWorld::ResolvePositions(std::vector<ContactManifold>& manifolds,
                                 int velocityIterations)
{
    for (auto& m : manifolds)
    {
        if (m.gRbA)
        {
            m.gRbA->SetTempPosition(
                m.gRbA->GetActor()->GetTransform()->GetLocalPosition());
        }
        if (m.gRbB)
        {
            m.gRbB->SetTempPosition(
                m.gRbB->GetActor()->GetTransform()->GetLocalPosition());
        }
    }

    // 2. 位置のイテレーション（めり込みを解消する）
    for (int i = 0; i < velocityIterations; ++i)
    {
        for (auto& m : manifolds)
        {
            // Rigidbodyの取得（付いていない場合は nullptr）
            Rigidbody* rbA = m.gRbA;
            Rigidbody* rbB = m.gRbB;

            // 質量の逆数を取得。Rigidbodyがない（静的）場合は 0.0f と見なす
            float invMassA = (rbA != nullptr) ? rbA->GetInverseMass() : 0.0f;
            float invMassB = (rbB != nullptr) ? rbB->GetInverseMass() : 0.0f;

            // 両方の InverseMass の合計
            float sumInvMass = invMassA + invMassB;

            // どちらも動かない物体（合計がゼロ）なら何もしない
            if (sumInvMass <= 0.0001f)
                continue;

            // めり込み量に対して、少し余裕（0.01fなど）を残して押し出す（ジッター対策）
            const float slop = 0.01f;
            const float percent =
                0.8f; // 1.0にすると跳ねすぎることがあるので80%程度にする
            float correctionMagnitude =
                std::max(m.gPenetration - slop, 0.0f) * percent;

            Vector3 correction = m.gNormal * (correctionMagnitude / sumInvMass);

            if (m.gRbA)
            {
                m.gRbA->SetTempPosition(m.gRbA->GetTempPosition() -
                                        (correction * invMassA));
            }

            if (m.gRbB)
            {
                m.gRbB->SetTempPosition(m.gRbB->GetTempPosition() +
                                        (correction * invMassB));
            }
            // 最終的なめり込み量を更新（残った分だけ次のイテレーションで解消する）
            m.gPenetration -= correctionMagnitude;
        }
    }
    for (auto& m : manifolds)
    {
        if (m.gRbA)
        {
            m.gRbA->GetActor()->GetTransform()->SetLocalPosition(
                m.gRbA->GetTempPosition());
            m.gRbA->GetActor()->GetTransform()->ComputeWorldTransform();
        }
        if (m.gRbB)
        {
            m.gRbB->GetActor()->GetTransform()->SetLocalPosition(
                m.gRbB->GetTempPosition());
            m.gRbB->GetActor()->GetTransform()->ComputeWorldTransform();
        }
    }
}

bool PhysWorld::CollectContactPoints_OBB_OBB(
    const OBB& a, const OBB& b, std::vector<ContactPoint>& outContacts,
    float contactOffset)
{
    Vector3 normal;
    float   depth;
    Vector3 singlePoint;

    if (!GetContactInfo_OBB(a, b, normal, depth, singlePoint))
    {
        return false;
    }

    // 軸を取得するための簡易ラムダ
    auto getLocalAxis = [](int i)
    {
        return (i == 0) ? Vector3::UnitX
                        : ((i == 1) ? Vector3::UnitY : Vector3::UnitZ);
    };

    // 2. Reference OBB（受ける側）と Incident OBB（ぶつかる側）を決める
    float dotA = 0.0f, dotB = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        Vector3 axisA = Vector3::Transform(getLocalAxis(i), a.mRotation);
        float   d     = std::abs(Vector3::Dot(axisA, normal));
        if (d > dotA)
            dotA = d;

        Vector3 axisB = Vector3::Transform(getLocalAxis(i), b.mRotation);
        d             = std::abs(Vector3::Dot(axisB, normal));
        if (d > dotB)
            dotB = d;
    }

    // 法線により直角に近い面を持っている方をReference（基準）とする
    bool       isARef = (dotA >= dotB);
    const OBB& refOBB = isARef ? a : b;
    const OBB& incOBB = isARef ? b : a;
    Vector3    refNormal =
        isARef ? normal : -1.0f * normal; // 常にReferenceから外を向く法線

    // 3. Incident Face（ぶつかってくる側の4頂点）の取得
    std::vector<Vector3> incFace = GetOBBIncidentFace(incOBB, refNormal);

    // 4. Reference OBB のサイド4面によるクリッピング（はみ出た部分をカット）
    Vector3 rAxes[3] = {Vector3::Transform(Vector3::UnitX, refOBB.mRotation),
                        Vector3::Transform(Vector3::UnitY, refOBB.mRotation),
                        Vector3::Transform(Vector3::UnitZ, refOBB.mRotation)};

    // Reference面の法線軸を特定
    int   nAxis = 0;
    float maxD  = 0;
    for (int i = 0; i < 3; ++i)
    {
        float d = std::abs(Vector3::Dot(rAxes[i], refNormal));
        if (d > maxD)
        {
            maxD  = d;
            nAxis = i;
        }
    }

    Vector3 axis1 = rAxes[(nAxis + 1) % 3];
    Vector3 axis2 = rAxes[(nAxis + 2) % 3];
    float   ext1 =
        ((nAxis + 1) % 3 == 0)
            ? refOBB.mExtents.x
            : (((nAxis + 1) % 3 == 1) ? refOBB.mExtents.y : refOBB.mExtents.z);
    float ext2 =
        ((nAxis + 2) % 3 == 0)
            ? refOBB.mExtents.x
            : (((nAxis + 2) % 3 == 1) ? refOBB.mExtents.y : refOBB.mExtents.z);

    std::vector<Vector3> clippedFace;
    std::vector<Vector3> inputFace = incFace;

    // クッキーの型抜きのように、4つの側面で順番にカットしていく
    ClipPolygonAgainstPlane(inputFace, axis1, refOBB.mCenter + axis1 * ext1,
                            clippedFace);
    inputFace = clippedFace;
    ClipPolygonAgainstPlane(inputFace, -1.0f * axis1,
                            refOBB.mCenter - axis1 * ext1, clippedFace);
    inputFace = clippedFace;
    ClipPolygonAgainstPlane(inputFace, axis2, refOBB.mCenter + axis2 * ext2,
                            clippedFace);
    inputFace = clippedFace;
    ClipPolygonAgainstPlane(inputFace, -1.0f * axis2,
                            refOBB.mCenter - axis2 * ext2, clippedFace);

    // 5.
    // カットされた頂点のうち、実際にめり込んでいる点だけをContactPointとして採用
    float extN  = (nAxis == 0)
                      ? refOBB.mExtents.x
                      : ((nAxis == 1) ? refOBB.mExtents.y : refOBB.mExtents.z);
    float signN = (Vector3::Dot(rAxes[nAxis], refNormal) > 0.0f) ? 1.0f : -1.0f;
    Vector3 refPlanePoint = refOBB.mCenter + rAxes[nAxis] * signN * extN;

    bool added = false;
    for (const auto& pt : clippedFace)
    {
        // Reference面からの深さを測る
        float pen = Vector3::Dot(refNormal, refPlanePoint - pt);
        if (pen > -contactOffset)
        { // めり込んでいる（または許容範囲内）
            float finalPen = std::max(pen, 0.0f);
            outContacts.emplace_back(ContactPoint{normal, finalPen, pt});
            added = true;
        }
    }

    // 6.
    // エッジ同士の衝突（十字に重なるなど）で点が0個になった場合は、SATの1点でフォールバック
    if (!added)
    {
        outContacts.emplace_back(ContactPoint{normal, depth, singlePoint});
    }

    return true;
}

vector<Vector3> PhysWorld::GetOBBIncidentFace(const OBB&     obb,
                                              const Vector3& normal)
{
    Vector3 axes[3] = {Vector3::Transform(Vector3::UnitX, obb.mRotation),
                       Vector3::Transform(Vector3::UnitY, obb.mRotation),
                       Vector3::Transform(Vector3::UnitZ, obb.mRotation)};

    int   bestAxis = 0;
    float minDot   = Math::Infinity;
    float sign     = 1.0f;

    // 法線と「最も逆向き」の軸を探す（これがぶつかる面の法線になる）
    for (int i = 0; i < 3; ++i)
    {
        float dot = Vector3::Dot(axes[i], normal);
        if (dot < minDot)
        {
            minDot   = dot;
            bestAxis = i;
            sign     = 1.0f;
        }
        if (-dot < minDot)
        {
            minDot   = -dot;
            bestAxis = i;
            sign     = -1.0f;
        }
    }

    Vector3 faceNormal = axes[bestAxis] * sign;
    int     a1         = (bestAxis + 1) % 3;
    int     a2         = (bestAxis + 2) % 3;
    Vector3 axis1      = axes[a1];
    Vector3 axis2      = axes[a2];

    float extNormal = (bestAxis == 0)
                          ? obb.mExtents.x
                          : ((bestAxis == 1) ? obb.mExtents.y : obb.mExtents.z);
    float ext1      = (a1 == 0) ? obb.mExtents.x
                                : ((a1 == 1) ? obb.mExtents.y : obb.mExtents.z);
    float ext2      = (a2 == 0) ? obb.mExtents.x
                                : ((a2 == 1) ? obb.mExtents.y : obb.mExtents.z);

    Vector3 center = obb.mCenter + faceNormal * extNormal;

    // 4つの頂点を生成して返す
    std::vector<Vector3> face;
    face.push_back(center - axis1 * ext1 - axis2 * ext2);
    face.push_back(center + axis1 * ext1 - axis2 * ext2);
    face.push_back(center + axis1 * ext1 + axis2 * ext2);
    face.push_back(center - axis1 * ext1 + axis2 * ext2);
    return face;
}

void PhysWorld::ClipPolygonAgainstPlane(const std::vector<Vector3>& inPoly,
                                        const Vector3&              planeNormal,
                                        const Vector3&              planePoint,
                                        std::vector<Vector3>&       outPoly)
{
    outPoly.clear();
    if (inPoly.empty())
        return;

    Vector3 p1 = inPoly.back();
    float   d1 = Vector3::Dot(planeNormal, p1 - planePoint);

    for (size_t i = 0; i < inPoly.size(); ++i)
    {
        Vector3 p2 = inPoly[i];
        float   d2 = Vector3::Dot(planeNormal, p2 - planePoint);

        if (d1 <= 0.0f)
        { // p1が平面の内側にある
            outPoly.push_back(p1);
        }
        if (d1 * d2 < 0.0f)
        { // 線分が平面と交差する
            float t = d1 / (d1 - d2);
            outPoly.push_back(p1 + (p2 - p1) * t); // 交差点を作成
        }
        p1 = p2;
        d1 = d2;
    }
}

bool PhysWorld::GetContactInfo_OBB(const OBB& a, const OBB& b,
                                   Vector3& outNormal, float& outDepth,
                                   Vector3& contactPoint)
{
    Vector3 aAxes[3] = {Vector3::Transform(Vector3::UnitX, a.mRotation),
                        Vector3::Transform(Vector3::UnitY, a.mRotation),
                        Vector3::Transform(Vector3::UnitZ, a.mRotation)};

    Vector3 bAxes[3] = {Vector3::Transform(Vector3::UnitX, b.mRotation),
                        Vector3::Transform(Vector3::UnitY, b.mRotation),
                        Vector3::Transform(Vector3::UnitZ, b.mRotation)};

    Vector3 axes[15];
    int     axisCount = 0;

    for (int i = 0; i < 3; ++i)
    {
        axes[axisCount++] = aAxes[i];
    }
    for (int i = 0; i < 3; ++i)
    {
        axes[axisCount++] = bAxes[i];
    }

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            Vector3 cross = Vector3::Cross(aAxes[i], bAxes[j]);
            if (cross.LengthSq() > 1e-6f)
            {
                axes[axisCount++] = cross.Normalized();
            }
        }
    }

    float   minOverlap = Math::Infinity;
    Vector3 bestAxis   = Vector3::Zero;

    for (int i = 0; i < axisCount; ++i)
    {
        Vector3 axis = axes[i];
        float   minA, maxA, minB, maxB;
        ProjectOBB(a, axis, minA, maxA);
        ProjectOBB(b, axis, minB, maxB);

        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap <= 0.0f)
        {
            // 分離軸あり
            return false;
        }

        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            bestAxis   = axis;
        }
    }

    // より信頼できる方向を使う（localからtransformされた差ベクトル）
    Vector3 dir  = b.mCenter - a.mCenter;
    float   fDir = Vector3::Dot(dir, bestAxis);
    if (fDir < 0.0f)
    {
        bestAxis *= -1.0f;
    }

    outNormal = bestAxis;
    outDepth  = minOverlap;
    // 法線はBからAへ向かっているので、
    // Aにとって最も深く刺さっている点は「bestAxis方向」の頂点
    Vector3 supportA = GetSupportPoint(a, bestAxis);
    // Bにとって最も深く刺さっている点は「-bestAxis方向」の頂点
    Vector3 supportB = GetSupportPoint(b, -1.0f * bestAxis);
    // 2つの頂点の中点を近似的な接触点とする
    contactPoint = (supportA + supportB) * 0.5f;
    return true;
}

bool PhysWorld::CollectContactPoints_Sphere_Sphere(
    const Sphere& a, const Sphere& b, std::vector<ContactPoint>& outContacts,
    float contactOffset)
{
    Vector3 diff = b.mCenter - a.mCenter;
    float   dist = diff.Length();

    float radiusSum   = a.mRadius + b.mRadius;
    float penetration = radiusSum - dist;

    if (penetration + contactOffset > 0.0f)
    {
        Vector3 normal = (dist > 0.0001f) ? diff / dist : Vector3::UnitX;
        // 接触点のワールド座標を計算
        // Sphere A の中心から法線方向に Sphere A の半径分進んだ点 (法線は A
        // から B に向かう)
        Vector3 contactPoint = a.mCenter + normal * a.mRadius;

        // ContactPoint に position を追加
        outContacts.emplace_back(
            ContactPoint{normal, penetration, contactPoint});
        return true;
    }
    return false;
}

bool PhysWorld::CollectContactPoints_Capsule_Capsule(
    const Capsule& a, const Capsule& b, std::vector<ContactPoint>& outContacts,
    float contactOffset)
{
    float distSq    = LineSegment::MinDistSq(a.mSegment, b.mSegment);
    float radiusSum = a.mRadius + b.mRadius;

    if (distSq <= (radiusSum + contactOffset) * (radiusSum + contactOffset))
    {
        float dist        = std::sqrt(distSq);
        float penetration = radiusSum - dist;

        // 最近接点を計算
        Vector3 pa, pb;
        ClosestPtsBetweenSegments(a.mSegment, b.mSegment, pa,
                                  pb); // 最近接点計算

        Vector3 normal = pb - pa;
        if (normal.LengthSq() > 0.0001f)
            normal.Normalize();
        else
            normal = Vector3::UnitX;
        // 接触点のワールド座標
        // pA と pB の中間点（あるいは pA から a.mRadius 分進んだ点）
        Vector3 contactPoint = pa + normal * a.mRadius;

        // ContactPoint に position を追加
        outContacts.emplace_back(
            ContactPoint{normal, penetration, contactPoint});
        return true;
    }
    return false;
}

bool PhysWorld::CollectContactPoints_OBB_Sphere(
    const OBB& a, const Sphere& b, std::vector<ContactPoint>& outContacts,
    float contactOffset)
{
    Vector3 closest = ClosestPointOnOBB(b.mCenter, a);
    Vector3 diff    = b.mCenter - closest;
    float   distSq  = diff.LengthSq();
    float   radius  = b.mRadius;

    if (distSq <= (radius + contactOffset) * (radius + contactOffset))
    {
        float dist        = std::sqrt(distSq);
        float penetration = radius - dist;

        // 押し出し応答が必要なのは、めり込みが発生している場合のみ
        if (penetration > 0.0001f) // 真にめり込んでいるかチェック
        {
            Vector3 normal = diff / dist;

            // 接触点のワールド座標を計算
            // Sphereの中心から法線方向にSphereの半径分戻った点を接触点とする
            Vector3 contactPoint = b.mCenter - normal * radius;

            // 接触点を追加
            outContacts.emplace_back(
                ContactPoint{normal, penetration, contactPoint});
            return true;
        }
    }
    return false;
}

bool PhysWorld::CollectContactPoints_OBB_Capsule(
    const OBB& a, const Capsule& b, std::vector<ContactPoint>& outContacts,
    float contactOffset)
{
    const int steps     = 10;
    float     minDistSq = Math::Infinity;
    Vector3   bestPointOnSeg, bestPointOnOBB;

    for (int i = 0; i <= steps; ++i)
    {
        float   t = i / static_cast<float>(steps);
        Vector3 pointOnSeg =
            Vector3::Lerp(b.mSegment.mStart, b.mSegment.mEnd, t);
        Vector3 pointOnOBB = ClosestPointOnOBB(pointOnSeg, a);

        float distSq = (pointOnSeg - pointOnOBB).LengthSq();
        if (distSq < minDistSq)
        {
            minDistSq      = distSq;
            bestPointOnSeg = pointOnSeg;
            bestPointOnOBB = pointOnOBB;
        }
    }

    float radius = b.mRadius;
    if (minDistSq <= (radius + contactOffset) * (radius + contactOffset))
    {
        float dist        = std::sqrt(minDistSq);
        float penetration = radius - dist;

        Vector3 normal = bestPointOnSeg - bestPointOnOBB;
        if (normal.LengthSq() > 0.0001f)
            normal.Normalize();
        else
            normal = Vector3::UnitX;

        // 接触点のワールド座標
        // Capsule の線分上の最近接点から法線と逆方向に半径分戻った点
        Vector3 contactPoint = bestPointOnSeg - normal * radius;

        // ContactPoint に position を追加
        outContacts.emplace_back(
            ContactPoint{normal, penetration, contactPoint});
        return true;
    }
    return false;
}

bool PhysWorld::CollectContactPoints_Sphere_Capsule(
    const Sphere& a, const Capsule& b, std::vector<ContactPoint>& outContacts,
    float contactOffset)
{
    float distSq    = b.mSegment.MinDistSq(a.mCenter);
    float radiusSum = a.mRadius + b.mRadius;

    if (distSq <= (radiusSum + contactOffset) * (radiusSum + contactOffset))
    {
        Vector3 ab      = b.mSegment.mEnd - b.mSegment.mStart;
        float   abLenSq = ab.LengthSq();

        float t = Vector3::Dot(a.mCenter - b.mSegment.mStart, ab) / abLenSq;
        t       = Math::Clamp(t, 0.0f, 1.0f);

        Vector3 closest     = b.mSegment.mStart + ab * t;
        Vector3 diff        = a.mCenter - closest;
        float   dist        = std::sqrt(distSq);
        float   penetration = radiusSum - dist;

        Vector3 normal = (dist > 0.0001f) ? diff / dist : Vector3::UnitX;

        // 接触点のワールド座標
        // Sphere A の中心から法線と逆方向に Sphere A の半径分戻った点
        Vector3 contactPoint = a.mCenter - normal * a.mRadius;

        // ContactPoint に position を追加
        outContacts.emplace_back(
            ContactPoint{normal, penetration, contactPoint});
        return true;
    }
    return false;
}

void PhysWorld::ClearAllCollider()
{
    mCollider.clear();
    mColliderXAxis.clear();
}

void PhysWorld::AddCollider(Collider* box)
{
    mCollider.push_back(box);
    mColliderXAxis.emplace_back(box);
}

void PhysWorld::RemoveCollider(Collider* box)
{
    auto iter = std::find(mCollider.begin(), mCollider.end(), box);
    if (iter != mCollider.end())
    {
        // ベクトルの末尾にスワップし、
        // ポップオフします（コピーの消去を避けるため）
        std::iter_swap(iter, mCollider.end() - 1);
        mCollider.pop_back();
    }

    iter = std::find(mColliderXAxis.begin(), mColliderXAxis.end(), box);
    if (iter != mColliderXAxis.end())
    {
        // ベクトルの末尾にスワップし、
        // ポップオフします（コピーの消去を避けるため）
        std::iter_swap(iter, mColliderXAxis.end() - 1);
        mColliderXAxis.pop_back();
    }

    for (auto it = mPrevHitPairs.begin(); it != mPrevHitPairs.end();)
    {
        if (it->first == box->GetOwner() || it->second == box->GetOwner())
        {
            it = mPrevHitPairs.erase(it); // eraseは次のイテレータを返す
        }
        else
        {
            ++it;
        }
    }
}
