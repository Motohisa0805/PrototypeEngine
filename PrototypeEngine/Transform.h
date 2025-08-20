#pragma once
#include "Math.h"
#include "BaseScene.h"
#include "Component.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//オブジェクトの座標、回転、スケーリングの情報を管理
//UnityのTransformに近いクラス
class Transform
{
protected:
	// World Transform
	Matrix4								mWorldTransform;
	//Model Transform
	Matrix4								mModelTransform;

	//ワールド座標、回転、スケーリング
	Vector3								mPosition;

	Quaternion							mRotation;

	Vector3								mScale;

	//ローカル座標、回転、スケーリング
	Vector3								mLocalPosition;

	Vector3								mPositionOffset;

	Quaternion							mLocalRotation;

	//X、Y、Z軸の回転量
	float								mRotationAmountX;
	float								mRotationAmountY;
	float								mRotationAmountZ;

	Vector3								mLocalScale;

	bool								mRecomputeWorldTransform;

	//親オブジェクト
	class Transform*					mParentActor;
	//子オブジェクトの配列
	vector<class Transform*>			mChildActor;

	vector<class Component*>			mComponents;
public:
										Transform();

	virtual								~Transform();

	virtual const Matrix4&				GetWorldTransform() const { return mWorldTransform; }

	virtual void						SetWorldTransform(Matrix4 mat) { mWorldTransform = mat; }

	virtual const Matrix4&				GetLocalTransform() const { return mModelTransform; }

	virtual Vector3						GetForward() const { return Vector3::Transform(Vector3::UnitZ, mLocalRotation); }

	virtual Vector3						GetRight() const { return Vector3::Transform(Vector3::UnitX, mLocalRotation); }

	virtual Vector3						GetUp() const { return Vector3::Transform(Vector3::UnitY, mLocalRotation); }

	virtual void						RotateToNewForward(const Vector3& forward);

	virtual void						LookAt(const Vector3& targetPosition);

	// Getters/setters
	virtual const Vector3&				GetPosition() const { return mPosition; }

	virtual const Quaternion&			GetRotation() const { return mRotation; }

	virtual const Vector3&				GetScale() const { return mScale; }

	// PositionのGetters/setters
	virtual const Vector3&				GetLocalPosition() const { return mLocalPosition; }

	virtual void						SetLocalPosition(const Vector3& pos)
	{
		mLocalPosition = pos;
		mRecomputeWorldTransform = true;
	}
	//Positionを加算で足す関数
	virtual void						AddLocalPosition(const Vector3& pos)
	{
		mLocalPosition += pos;
		mRecomputeWorldTransform = true;
	}
	//mPositionOffsetに加算する関数
	virtual void						AddPositionOffset(const Vector3& pos)
	{
		mPositionOffset += pos;
	}
	// ScaleのGetters/setters
	virtual Vector3						GetLocalScale() const { return mLocalScale; }
	// 1 Ver
	virtual void						SetLocalScale(Vector3 scale)
	{
		mLocalScale = scale;
		mRecomputeWorldTransform = true;
	}
	// 2 Ver
	virtual void						SetLocalScale(float scale)
	{
		mLocalScale = Vector3(scale, scale, scale);
		mRecomputeWorldTransform = true;
	}
	// RotationのGetters/setters
	virtual const Quaternion&			GetLocalRotation() const { return mLocalRotation; }

	virtual void						SetLocalRotation(const Quaternion& rotation)
	{
		mLocalRotation = rotation;
		mRecomputeWorldTransform = true;
	}

	//軸別の回転量のGetters/setters
	virtual float						GetRotationAmountX() { return mRotationAmountX; }

	virtual float						GetRotationAmountY() { return mRotationAmountY; }

	virtual float						GetRotationAmountZ() { return mRotationAmountZ; }

	virtual void						SetRotationAmountX(float rot) { mRotationAmountX = rot; }

	virtual void						SetRotationAmountY(float rot) { mRotationAmountY = rot; }

	virtual void						SetRotationAmountZ(float rot) { mRotationAmountZ = rot; }

	//ワールド座標の更新		
	virtual void						ComputeWorldTransform(const class Matrix4* parentMatrix);

	virtual void						LocalBonePositionUpdateActor(Matrix4 boneMatrix, const class Matrix4& parentActor);

	//***子オブジェクト関係の処理***
	virtual class Transform*			GetParentActor() { return mParentActor; }

	virtual const class Transform*		GetChildActor(Transform* actor);

	// Add/remove components
	virtual void						AddComponent(Component* component);

	virtual void						RemoveComponent(Component* component);
	//子オブジェクトを追加
	virtual void						AddChildActor(Transform* actor);

	virtual void						RemoveChildActor(Transform* actor);


	virtual void						AddParentActor(Transform* parent);

	virtual void						RemoveParentActor();

	vector<class Component*>			GetComponents() { return mComponents; }

	//子オブジェクトの座標更新
	virtual void						SetActive() { mRecomputeWorldTransform = true; }
};

