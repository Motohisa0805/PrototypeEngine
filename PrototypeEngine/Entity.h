#pragma once
#include "BaseScene.h"
#include "Time.h"


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

class Component;

class Entity
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

	vector<Component*>				mComponents;
public:
	Entity(uint64_t id = 0);
	Entity(BaseScene* scene);
	~Entity();

	virtual void FixedUpdate(float deltaTime);

	virtual void FixedUpdateComponents(float deltaTime);

	virtual void Update(float deltaTime);

	virtual void EditorComputeWorldTransform();

	virtual void UpdateComponents(float deltaTime);

	virtual void StateUpdate(float deltaTime);

	virtual void ProcessInput(const struct InputState& keyState);

	virtual void ActorInput(const struct InputState& keyState);

	virtual void OnDestroy();

	virtual void OnEnabled();

	virtual void OnDisable();


	virtual void Serialize(json& j) const;

	virtual void Deserialize(const json& j);

	virtual void OnComponentAdded(Component* newComp);


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

	template<typename T>
	T*								GetComponent() const;
	// Add/remove components
	virtual void					AddComponent(Component* component);
	virtual void					RemoveComponent(Component* component);
	const vector<Component*>&		GetComponents()const { return mComponents; }

	// 通常生成時はIDを指定せず自動採番、ロード時はファイルから読み込んだIDを指定する

	uint64_t						GetID()const { return mID; }

	string							GetName() const { return mName; }

	void 							SetName(const string& name) { mName = name; }
};

template<typename T>
inline T* Entity::GetComponent() const
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