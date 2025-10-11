#pragma once
#include "Math.h"
#include "BaseScene.h"
#include "Component.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Matrix4;
//オブジェクトの座標、回転、スケーリングの情報を管理
//UnityのTransformに近いクラス
class Transform
{
protected:

	//ローカル座標、回転、スケーリング
	Vector3								mLocalPosition;
	Vector3								mPositionOffset;
	Quaternion							mLocalRotation;
	Vector3								mLocalScale;

	//計算結果をキャッシュ
	//これらはmWorldTransformから分解された値であり、直接編集してらだめ
	Vector3								mPosition;
	Quaternion							mRotation;
	Vector3								mScale;

	Matrix4								mLocalTransform;
	Matrix4								mWorldTransform;

	bool								mIsDirty;

	//親オブジェクト
	Transform*							mParentActor;
	//子オブジェクトの配列
	vector<Transform*>					mChildActor;

	//X、Y、Z軸の回転量
	float								mRotationAmountX;
	float								mRotationAmountY;
	float								mRotationAmountZ;


	vector<Component*>					mComponents;
public:
										Transform();

	virtual								~Transform();

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
		SetDirty();
	}


	//軸別の回転量のGetters/setters
	virtual float						GetRotationAmountX() { return mRotationAmountX; }
	virtual float						GetRotationAmountY() { return mRotationAmountY; }
	virtual float						GetRotationAmountZ() { return mRotationAmountZ; }
	virtual void						SetRotationAmountX(float rot) { mRotationAmountX = rot; }
	virtual void						SetRotationAmountY(float rot) { mRotationAmountY = rot; }
	virtual void						SetRotationAmountZ(float rot) { mRotationAmountZ = rot; }

	//ワールド座標の更新		
	virtual void						ComputeWorldTransform();

	virtual void						LocalBonePositionUpdateActor(Matrix4 boneMatrix, const Matrix4& parentActor);

	//***子オブジェクト関係の処理***
	virtual Transform*					GetParentActor() { return mParentActor; }

	virtual const Transform*			GetChildActor(Transform* actor);

	//子オブジェクトを追加
	virtual void						AddChildActor(Transform* actor);

	virtual void						RemoveChildActor(Transform* actor);


	virtual void						AddParentActor(Transform* parent);

	virtual void						RemoveParentActor();

	// Add/remove components
	virtual void						AddComponent(Component* component);
	virtual void						RemoveComponent(Component* component);
	const vector<Component*>&			GetComponents()const { return mComponents; }

	//子オブジェクトの座標更新
	virtual void						SetDirty();


	// JSONに変換するメソッド
	virtual void						Serialize(json& j) const;
	// JSONから復元するメソッド
	virtual void						Deserialize(const json& j);
};

