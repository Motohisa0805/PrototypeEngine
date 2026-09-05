#pragma once
#include "MeshRenderer.h"
#include "Animator.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Animator;
class SkeletonData;

//スケルタルメッシュの描画を行うクラス
//MeshRendererと同様スケルタルメッシュを読み込んで使用する
class SkeletalMeshRenderer : public MeshRenderer
{
public:
	struct Bounds
	{
        Vector3 sCenter;
        Vector3 sExtent;
	};

protected:

	SkeletonData*					mSkeletonData;
    vector<ActorObject*>			mBones;
    MatrixPalette                   mPalette;

	//***プロパティ変数***
    Bounds							mBounds;

	Transform*						mRootBone;

public:
									SkeletalMeshRenderer(Entity* owner);
									~SkeletalMeshRenderer();
	// スケルタルモデルの描画
	bool							Draw(Shader* shader) override;

	void							DrawForShadowMap(Shader* shader)override;

	void							Update(float deltaTime) override;
	
	void							LoadSkeletonMesh(const char* path, const char* localID,ActorObject* rootBone);

	ActorObject*					FindActorByName(ActorObject* current, const string& name);

	// Setters
	void							SetSkeleton(SkeletonData* sk, ActorObject* actor);

	SkeletonData*					GetSkeleton() { return mSkeletonData; }

	Bounds							GetBounds() { return mBounds; }

	Transform*						GetRootBone() { return mRootBone; }

	void							Serialize(json& j) const override;
	void							Deserialize(const json& j)override;

	void							DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*						Clone(Entity* newOwner) const override;
};