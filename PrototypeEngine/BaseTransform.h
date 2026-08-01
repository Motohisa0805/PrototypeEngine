#pragma once
#include "Math.h"
#include "BaseScene.h"
#include "Component.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

class BaseTransform : public Component
{
protected:

	//ローカル座標、回転、スケーリング
	Vector3								mLocalPosition;
	Vector3								mPositionOffset;
	Quaternion							mLocalRotation;
	//回転をVector3で保持(GUIのキャッシュ用)
	Vector3								mLocalEulerAngles;

	Vector3								mLocalScale;

	//計算結果をキャッシュ
	//これらはmWorldTransformから分解された値であり、直接編集してらだめ
	Vector3								mPosition;
	Quaternion							mRotation;
	Vector3								mScale;

	Matrix4								mLocalTransform;
	Matrix4								mWorldTransform;

	bool								mIsDirty;

	uint64_t							mParentID;
public:
	BaseTransform(class Entity* owner);

	virtual								~BaseTransform();

	virtual const Matrix4&				GetWorldTransform() const { return mWorldTransform; }

	virtual void						SetWorldTransform(Matrix4 mat) { mWorldTransform = mat; }

	virtual const Matrix4&				GetLocalTransform() const { return mLocalTransform; }

	virtual Vector3						GetForward() const { return Vector3::Transform(Vector3::UnitZ, mLocalRotation); }

	virtual Vector3						GetRight() const { return Vector3::Transform(Vector3::UnitX, mLocalRotation); }

	virtual Vector3						GetUp() const { return Vector3::Transform(Vector3::UnitY, mLocalRotation); }

	virtual void						RotateToNewForward(const Vector3& forward);

	virtual void						LookAt(const Vector3& targetPosition);

	// Getters/setters
	virtual const Vector3&				GetPosition()
	{
		ComputeWorldTransform(); // 必要なら更新
		return mPosition;
	}
	virtual void						SetPosition(const Vector3& pos);
	virtual const Quaternion&			GetRotation()
	{
		ComputeWorldTransform(); // 必要なら更新
		return mRotation;
	}
	virtual void						SetRotation(const Quaternion& rotation);
	virtual const Vector3&				GetScale()
	{
		ComputeWorldTransform(); // 必要なら更新
		return mScale;
	}
	// 1 Ver
	virtual void						SetScale(Vector3 scale);
	// 2 Ver
	virtual void						SetScale(float scale)
	{
		mScale = Vector3(scale, scale, scale);
		SetDirty();
	}

	// PositionのGetters/setters
	virtual const Vector3&				GetLocalPosition() const { return mLocalPosition; }
	virtual void						SetLocalPosition(const Vector3& pos)
	{
		mLocalPosition = pos;
		SetDirty();
	}
	// ScaleのGetters/setters
	virtual Vector3						GetLocalScale() const { return mLocalScale; }
	// 1 Ver
	virtual void						SetLocalScale(Vector3 scale)
	{
		mLocalScale = scale;
		SetDirty();
	}
	// 2 Ver
	virtual void						SetLocalScale(float scale)
	{
		mLocalScale = Vector3(scale, scale, scale);
		SetDirty();
	}
	// RotationのGetters/setters
	virtual const Quaternion&			GetLocalRotation() const { return mLocalRotation; }

	virtual void						SetLocalRotation(const Quaternion& rotation)
	{
		mLocalRotation = rotation;
        Vector3 eulerRad = mLocalRotation.ToEulerAngles();
        mLocalEulerAngles.x = Math::ToDegrees(eulerRad.x);
        mLocalEulerAngles.y = Math::ToDegrees(eulerRad.y);
        mLocalEulerAngles.z = Math::ToDegrees(eulerRad.z);
		SetDirty();
	}

	Vector3								GetLocalEulerAngles() { return mLocalEulerAngles; }
	void								SetLocalEulerAngles(const Vector3& angles)
	{
        mLocalEulerAngles = angles;

		Quaternion qy = Quaternion::CreateFromAxisAngle(Vector3::UnitY,mLocalEulerAngles.y);
		Quaternion qx = Quaternion::CreateFromAxisAngle(Vector3::UnitX,mLocalEulerAngles.x);
		Quaternion qz = Quaternion::CreateFromAxisAngle(Vector3::UnitZ,mLocalEulerAngles.z);

		mLocalRotation = qy * qx * qz;
        SetDirty();
	}

	//ワールド座標の更新		
	virtual void						ComputeWorldTransform(){}

	//子オブジェクトの座標更新
	virtual void						SetDirty(){}
	virtual void						ActiveDirty(){}

	uint64_t							GetParentID() { return mParentID; }
	void								SetParentID(uint64_t id) { mParentID = id; }
};

