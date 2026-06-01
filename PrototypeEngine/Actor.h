#pragma once
#include "Entity.h"
#include "Transform.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//ゲームオブジェクトのアクター
class ActorObject : public Entity
{
protected:
	Transform*						mTransform;
public:
	//コンストラクタ
									ActorObject(uint64_t id = 0);
									ActorObject(BaseScene* scene);
	//デストラクタ
	virtual							~ActorObject();
	// ゲームから一定数で呼び出される更新関数（オーバーライド不可）
	void							FixedUpdate(float deltaTime)override;
	// Actorに接続されたすべてのコンポーネントを更新します（オーバーライド不可）
	void							FixedUpdateComponents(float deltaTime)override;
	// ゲームから呼び出される更新関数（オーバーライド不可）
	void							Update(float deltaTime)override;
	void							EditorComputeWorldTransform()override;


	Transform*						GetTransform() const { return mTransform; }
	BaseTransform*					GetBaseTransform()override { return mTransform; }

	//親のアクターのGetter
	//当たった時に呼び出される関数
	virtual void					OnCollisionEnter(ActorObject* target);
	//当たっている時に呼び出される関数
	virtual void					OnCollisionStay(ActorObject* target);
	//当たり終わった時に呼び出される関数
	virtual void					OnCollisionExit(ActorObject* target);


	// JSONに変換するメソッド
	void							Serialize(json& j) const;
	// JSONから復元するメソッド
	void							Deserialize(const json& j)override;

	void							LoadParentByLoadScene();

	Entity*							Clone()override;
};
