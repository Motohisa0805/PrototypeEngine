#pragma once
#include "Math.h"
#include "Actor.h"


/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍元を改造したファイル
//スケルタルモデルのボーンのトランスフォームクラス
//ボーンの位置、回転、スケーリングを管理するクラス
class BoneTransform
{
private:
	// 現状はデータを公開にしています。
	//また後々整理はする予定です。
	Vector3					mScale;

	Quaternion				mRotation;
	
	Vector3					mPosition;
public:
	Matrix4					ToMatrix() const;

	void					FromMatrix(Matrix4& mat);

	static BoneTransform	Interpolate(const BoneTransform& a, const BoneTransform& b, float f);

	Vector3					GetScale() { return mScale; }

	void					SetScale(Vector3 scale) { mScale = scale; }

	Quaternion				GetRotation() { return mRotation; }

	void					SetRotation(Quaternion rotation) { mRotation = rotation; }

	Vector3					GetPosition() { return mPosition; }

	void					SetPosition(Vector3 position) { mPosition = position; }
};