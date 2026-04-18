#pragma once
#include "BaseScene.h"
#include "Transform.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

enum ActorTag
{
	None,
	Player,
	Ground,
	Enemy,
	Recovery
};

//前方宣言
class Rigidbody;
class Collider;
//全3Dモデルの基底クラス
class ActorObject
{
public:
	//オブジェクトの状態
	enum State
	{
		EActive,
		EPaused,
		EDead
	};
protected:
	string						mName;
	// Actor's state
	State						mState;

	State						mPreviousState;

	//オブジェクトのタグ
	ActorTag					mActorTag = ActorTag::None;

	BaseScene*					mGame;

	Transform*					mTransform;

	Rigidbody*					mRigidbody;

	Collider*					mCollider;

	vector<Component*>			mComponents;
public:
	//コンストラクタ
								ActorObject();
								ActorObject(BaseScene* scene);
	//デストラクタ
	virtual						~ActorObject();
	// ゲームから一定数で呼び出される更新関数（オーバーライド不可）
	void						FixedUpdate(float deltaTime);
	// Actorに接続されたすべてのコンポーネントを更新します（オーバーライド不可）
	void						FixedUpdateComponents(float deltaTime);
	// ゲームから呼び出される更新関数（オーバーライド不可）
	void						Update(float deltaTime);
	void						EditorComputeWorldTransform();
	// Actorに接続されたすべてのコンポーネントを更新します（オーバーライド不可）
	void						UpdateComponents(float deltaTime);

	void						StateUpdate(float deltaTime);
	// ゲームから呼び出されたProcessInput関数（オーバーライドできません）
	void						ProcessInput(const struct InputState& keyState);
	// 任意のActor特有の入力コード（上書き可能）
	virtual void				ActorInput(const struct InputState& keyState);
	// Actorが破壊されたときに呼び出される関数
	void						OnDestroy();

	// Getters/setters
	State						GetState() const { return mState; }

	void						SetState(State state) { mState = state; }

	State 						GetPreviousState() const { return mPreviousState; }

	void 						SetPreviousState(State state) { mPreviousState = state; }

	virtual ActorTag			GetActorTag() { return mActorTag; }

	void						SetActorTag(ActorTag tag) { mActorTag = tag; }
	
	BaseScene*					GetGame() { return mGame; }

	Transform*					GetTransform() const { return mTransform; }

	Rigidbody*					GetRigidbody() { return mRigidbody; }

	Collider*					GetCollider() { return mCollider; }

	template<typename T>
	T*							GetComponent() const;
	// Add/remove components
	virtual void				AddComponent(Component* component);
	virtual void				RemoveComponent(Component* component);
	const vector<Component*>&	GetComponents()const { return mComponents; }


	//親のアクターのGetter
	//当たった時に呼び出される関数
	virtual void				OnCollisionEnter(ActorObject* target){}
	//当たっている時に呼び出される関数
	virtual void				OnCollisionStay(ActorObject* target){}
	//当たり終わった時に呼び出される関数
	virtual void				OnCollisionExit(ActorObject* target){}

	string						GetName() const { return mName; }

	void 						SetName(const string& name) { mName = name; }


	// JSONに変換するメソッド
	void						Serialize(json& j) const;
	// JSONから復元するメソッド
	void						Deserialize(const json& j);
	//コンポーネントが追加された後に呼ばれる通知関数
	virtual void				OnComponentAdded(Component* newComp);
};

template<typename T>
inline T* ActorObject::GetComponent() const
{
	for (auto component : mComponents)
	{
		if (T* casted = dynamic_cast<T*>(component))
		{
			return casted;
		}
	}
	return nullptr; // 指定した型のコンポーネントが見つからなかった場合
}
