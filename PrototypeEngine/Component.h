#pragma once
#include <cstdint>
#include "Typedefs.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//コンポーネントの基底クラス
// Unityのコンポーネントに近いクラス
class Component
{
protected:
	// コンポーネント元のオブジェクト
	class ActorObject*	mOwner;
	class BaseScene*	mGame;
	// コンポーネントの順序を更新する
	int					mUpdateOrder;

	string				mName;
public:
	// コンストラクタ
	// （更新順序が低いほど、コンポーネントが早く更新される）
						Component(class ActorObject* owner, int updateOrder = 100);
	// デストラクター
	virtual				~Component();
	virtual void		FixedUpdate(float deltaTime);
	// デルタ時間でこのコンポーネントを更新してください
	virtual void		Update(float deltaTime);
	// このコンポーネントの入力を処理する
	virtual void		ProcessInput(const struct InputState& keyState) {}
	// 世界の変化によって呼び出される
	virtual void		OnUpdateWorldTransform() { }

	class ActorObject*	GetOwner() { return mOwner; }
	int					GetUpdateOrder() const { return mUpdateOrder; }

	string				GetName()const { return mName; }

	// JSONに変換するメソッド
	virtual void		Serialize(json& j) const;
	// JSONから復元するメソッド
	virtual void		Deserialize(const json& j);

	virtual void		DrawGUI(){}
};