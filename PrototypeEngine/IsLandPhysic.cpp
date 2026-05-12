#include "IsLandPhysic.h"
#include "PhysWorld.h"


IsLandPhysic::IsLandPhysic(PhysWorld* world)
	:mWorld(world)
	, mActiveBodies()
{
}

void IsLandPhysic::BuildAndSolveIslands(std::vector<ContactManifold>& manifolds, float deltaTime)
{
    // 訪問フラグの管理
    // そのフレームで、どのRigidbodyがすでに島に振り分けられたか
    std::unordered_set<Rigidbody*> visited;
    // どの衝突（マニホールド）がすでに島に組み込まれたか
    std::unordered_set<ContactManifold*> processedManifolds;

    // 起点となる「動いている物体」を一つずつ確認
    for (Rigidbody* rb : mActiveBodies) {
        // すでにどこかの島に含まれている、または静止物ならスキップ
        if (visited.count(rb) || rb->GetInverseMass() == 0.0f) continue;

        // --- 新しい島（Island）の構築開始 ---
        std::vector<Rigidbody*> islandBodies;
        std::vector<ContactManifold*> islandManifolds;
        std::stack<Rigidbody*> stack;

        // 起点をスタックに入れる
        stack.push(rb);
        visited.insert(rb);

        // DFS（深さ優先探索）で繋がっている物体をすべて集める
        while (!stack.empty()) {
            Rigidbody* current = stack.top();
            stack.pop();
            islandBodies.push_back(current);

            // 全ての衝突情報を走査して、currentに関連するもの（隣人）を探す
            for (auto& m : manifolds) {
                if (m.gProcessed) continue;
                // すでにこの衝突が別の島（または探索中）で処理されていたら無視
                if (processedManifolds.count(&m)) continue;

                // currentが衝突ペア（AまたはB）に含まれているかチェック
                Rigidbody* neighbor = nullptr;
                bool isRelated = false;

                if (m.gRbA == current) 
                {
                    isRelated = true; 
                    neighbor = m.gRbB;
                }
                else if (m.gRbB == current) 
                {
                    isRelated = true;
                    neighbor = m.gRbA;
                }

                if (isRelated) {
					islandManifolds.push_back(&m);
					m.gProcessed = true; // この衝突はこの島で処理することをマーク
                    // neighborが存在し、かつ未訪問の「動く物体」の場合のみ、探索を先に進める
                    if (neighbor && neighbor->GetInverseMass() > 0.0f) {
                        if (visited.count(neighbor) == 0) {
                            visited.insert(neighbor);
                            stack.push(neighbor);
                        }
                    }
                }
            }
        }

        // 4. 完成した「島」単位で物理シミュレーションを実行！
        if (!islandBodies.empty()) {
            SolveIsland(islandBodies, islandManifolds, deltaTime);
        }
    }
}

void IsLandPhysic::SolveIsland(vector<Rigidbody*>& islandBodies, vector<ContactManifold*>& manifolds, float deltaTime)
{
	const int velocityIterations = Physics::VELOCITY_ITERATIONS; // 速度（跳ね返り・摩擦）の反復回数
	const int positionIterations = Physics::POSITION_ITERATIONS; // 位置（めり込み押し出し）の反復回数
	// 1. 速度のイテレーション（これを繰り返すとジェンガが安定する）
	for (int i = 0; i < velocityIterations; ++i) {
		for (auto& m : manifolds) {
			for (auto& point : m->gContactPoints) {
				if (m->gRbA) {
					m->gRbA->ResolveVelocity(m->gRbB, m->gNormal, point, deltaTime);
				}
				else if (m->gRbB) {
					m->gRbB->ResolveVelocity(nullptr, m->gNormal, point, deltaTime);
				}
			}
		}
	}
	// 2. 位置のイテレーション（めり込みを解消する）
	for (int i = 0; i < positionIterations; ++i) {
		for (auto& m : manifolds) {
			mWorld->OneResolvePosition(*m);
		}
	}
}

void IsLandPhysic::AddActiveBodies(Rigidbody* rb)
{
	if (rb->IsInActiveList()) return; // すでにアクティブリストにある場合は追加しない
	rb->SetInActiveList(true);
	mActiveBodies.push_back(rb);
}

void IsLandPhysic::RemoveActioveBodies(Rigidbody* rb)
{
	auto iter = find(mActiveBodies.begin(), mActiveBodies.end(), rb);
	if (iter != mActiveBodies.end()) {
		rb->SetInActiveList(false);
		mActiveBodies.erase(iter);
	}
}
