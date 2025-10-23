#pragma once
#include "Component.h"

class ScriptComponent : public Component
{
private:
	bool mHasStarted = false;
public:
	ScriptComponent(class ActorObject* owner);

	virtual ~ScriptComponent();


	// ----------------------------------------------------
	// ライフサイクル関数 (ユーザーがオーバーライドする場所)
	// ----------------------------------------------------

	//初期化処理:最初のUpdate()が呼ばれる前に一度だけ呼ばれる
	virtual void Start(){}

	//毎フレームの更新処理
	virtual void Update(float deltaTime)override{}

	//物理更新処理
	virtual void FixedUpdate(float deltaTime)override{}

	bool HasStarted()const { return mHasStarted; }
	void SetStarted(bool started) { mHasStarted = started; }
};

