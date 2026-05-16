#pragma once
#include "BaseScene.h"
#include "Transform.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//アクターのタグ、レイヤーなど詳細情報をまとめるnamespace
namespace ActorInformation
{
	enum Tag
	{
		None,
		Player,
		Ground,
		Enemy,
		Recovery
	};

	//オブジェクトの静的状態enum class
	enum class StaticTag : uint32_t
	{
		Nothing = 0,
		Occluder_Static = 1 << 0,//遮蔽する側
		Occludee_Static = 1 << 1,//遮蔽される側
		Everything = Occluder_Static | Occludee_Static
	};
	// 演算子オーバーロード（OR演算のみ）
	inline StaticTag operator|(StaticTag a, StaticTag b) {
		return static_cast<StaticTag>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
			);
	}

	inline string GetStateName(StaticTag tag)
	{
		switch (tag)
		{
		case ActorInformation::StaticTag::Nothing:
			return "Nothing";
		case ActorInformation::StaticTag::Occluder_Static:
			return "Occluder_Static";
		case ActorInformation::StaticTag::Occludee_Static:
			return "Occludee_Static";
		case ActorInformation::StaticTag::Everything:
			return "Everything";
		default:
			break;
		}
	} 
};

//前方宣言
class Rigidbody;
class Collider;
//ゲームオブジェクトのアクター
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
	uint64_t						mID;
	string							mName;
	// Actor's state
	State							mState;

	State							mPreviousState;

	//オブジェクトのタグ
	ActorInformation::Tag			mActorTag = ActorInformation::Tag::None;

	//オブジェクトの静的状態タグ
	ActorInformation::StaticTag		mStatic = ActorInformation::StaticTag::Nothing;

	BaseScene*						mGame;

	Transform*						mTransform;

	Rigidbody*						mRigidbody;

	Collider*						mCollider;

	vector<Component*>				mComponents;
public:
	//コンストラクタ
									ActorObject(uint64_t id = 0);
									ActorObject(BaseScene* scene);
	//デストラクタ
	virtual							~ActorObject();
	// ゲームから一定数で呼び出される更新関数（オーバーライド不可）
	void							FixedUpdate(float deltaTime);
	// Actorに接続されたすべてのコンポーネントを更新します（オーバーライド不可）
	void							FixedUpdateComponents(float deltaTime);
	// ゲームから呼び出される更新関数（オーバーライド不可）
	void							Update(float deltaTime);
	void							EditorComputeWorldTransform();
	// Actorに接続されたすべてのコンポーネントを更新します（オーバーライド不可）
	void							UpdateComponents(float deltaTime);

	void							StateUpdate(float deltaTime);
	// ゲームから呼び出されたProcessInput関数（オーバーライドできません）
	void							ProcessInput(const struct InputState& keyState);
	// 任意のActor特有の入力コード（上書き可能）
	virtual void					ActorInput(const struct InputState& keyState);
	// Actorが破壊されたときに呼び出される関数
	void							OnDestroy();
	void							OnEnabled();
	//一時的に機能停止
	void							OnDisable();
	// Getters/setters
	State							GetState() const { return mState; }

	void							SetState(State state) { mState = state; }

	State 							GetPreviousState() const { return mPreviousState; }

	void 							SetPreviousState(State state) { mPreviousState = state; }

	virtual ActorInformation::Tag	GetActorTag() { return mActorTag; }

	void							SetActorTag(ActorInformation::Tag tag) { mActorTag = tag; }

	ActorInformation::StaticTag		GetStatic() { return mStatic; }

	void							SetStaticTag(ActorInformation::StaticTag statics) { mStatic = statics; }
	//静的フラグがONなら
	bool							IsStatic() { return mStatic != ActorInformation::StaticTag::Nothing; }
	
	BaseScene*						GetGame() { return mGame; }

	Transform*						GetTransform() const { return mTransform; }

	Rigidbody*						GetRigidbody() { return mRigidbody; }

	Collider*						GetCollider() { return mCollider; }

	template<typename T>
	T*								GetComponent() const;
	// Add/remove components
	virtual void					AddComponent(Component* component);
	virtual void					RemoveComponent(Component* component);
	const vector<Component*>&		GetComponents()const { return mComponents; }


	//親のアクターのGetter
	//当たった時に呼び出される関数
	virtual void					OnCollisionEnter(ActorObject* target);
	//当たっている時に呼び出される関数
	virtual void					OnCollisionStay(ActorObject* target);
	//当たり終わった時に呼び出される関数
	virtual void					OnCollisionExit(ActorObject* target);

	// 通常生成時はIDを指定せず自動採番、ロード時はファイルから読み込んだIDを指定する

	uint64_t						GetID()const { return mID; }

	string							GetName() const { return mName; }

	void 							SetName(const string& name) { mName = name; }


	// JSONに変換するメソッド
	void							Serialize(json& j) const;
	// JSONから復元するメソッド
	void							Deserialize(const json& j);
	//コンポーネントが追加された後に呼ばれる通知関数
	virtual void					OnComponentAdded(Component* newComp);


	ActorObject*					Clone();
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
