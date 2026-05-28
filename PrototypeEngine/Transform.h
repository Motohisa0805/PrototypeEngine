#pragma once
#include "BaseTransform.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//コンポーネントに近い扱いとして作成
//※通常のコンポーネントとは違いオブジェクト生成時に自動的に生成されるように設計

//前方宣言
class Matrix4;
class ActorObject;

class Transform : public BaseTransform
{
protected:

	//親オブジェクト
	ActorObject*						mParentActor;
	//子オブジェクトの配列
	vector<ActorObject*>				mChildActor;

	// これらのヘルパー関数は private にして SetParent からのみ呼び出すようにすると設計が綺麗になります
	void								AddChild(ActorObject* child);
	void								RemoveChild(ActorObject* child);
public:
	Transform(class ActorObject* owner);

	virtual								~Transform();

	//ワールド座標の更新		
	void								ComputeWorldTransform()override;

	//***子オブジェクト関係の処理***
	virtual ActorObject*				GetParentActor() { return mParentActor; }

	virtual const ActorObject*			GetChildActor(ActorObject* actor);

	const vector<ActorObject*>			GetChildActorList()const { return mChildActor; }
	vector<ActorObject*>&				GetChildActorListMutable() { return mChildActor; }

	//子オブジェクトを追加
	virtual void						AddChildActor(ActorObject* childtransform);

	virtual void						RemoveChildActor(ActorObject* childtransform);


	virtual void						AddParentActor(ActorObject* parent);
	virtual void						SetParent(ActorObject* newParent);

	virtual void						RemoveParentActor();

	//子オブジェクトの座標更新
	void								SetDirty()override;
	void								ActiveDirty()override;


	void								Serialize(json& j) const override;
	void								Deserialize(const json& j)override;

	void								DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;
};

