#pragma once
#include "Entity.h"
#include "RectTransform.h"
#include "Shader.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

class UIActorObject : public Entity
{
protected:
	RectTransform*					mRectTransform;
public:
									UIActorObject(uint64_t id = 0);
									UIActorObject(BaseScene* scene);
	virtual							~UIActorObject();
	// ゲームから一定数で呼び出される更新関数（オーバーライド不可）
	void							FixedUpdate(float deltaTime)override;
	// Actorに接続されたすべてのコンポーネントを更新します（オーバーライド不可）
	void							FixedUpdateComponents(float deltaTime)override;
	// ゲームから呼び出される更新関数（オーバーライド不可）
	void							Update(float deltaTime)override;
	void							EditorComputeWorldTransform()override;

	RectTransform*					GetRectTransform() { return mRectTransform; }

	// JSONに変換するメソッド
	void							Serialize(json& j) const;
	// JSONから復元するメソッド
	void							Deserialize(const json& j)override;

	UIActorObject*					Clone();
};

