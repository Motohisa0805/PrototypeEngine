#pragma once
#include "Component.h"
#include "ComponentFactory.h"
#include "GameScripts/GameScripts_API.h"

class GAMESCRIPTS_API ScriptComponent : public Component
{
private:
	bool mHasStarted = false;
public:
	ScriptComponent(class ActorObject* owner);

	virtual ~ScriptComponent();

	bool HasStarted()const { return mHasStarted; }
	void SetStarted(bool started) { mHasStarted = started; }

	// ----------------------------------------------------
	// ライフサイクル関数 (ユーザーがオーバーライドする場所)
	// ----------------------------------------------------

	//初期化処理:Startの前に呼ばれるものとして宣言
	virtual void Awake(){}

	//初期化処理:最初のUpdate()が呼ばれる前に一度だけ呼ばれる
	virtual void Start(){}

	//毎フレームの更新処理
	virtual void Update(float deltaTime)override{}

	//物理更新処理
	virtual void FixedUpdate(float deltaTime)override{}

};

