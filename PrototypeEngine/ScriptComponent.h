#pragma once
#include "InGameProject/InGameProject_API.h"
#include "Component.h"
#include "ComponentFactory.h"

class ScriptComponent : public Component
{
private:
	bool mHasStarted = false;
public:
	GAMESCRIPTS_API ScriptComponent(class ActorObject* owner);

	GAMESCRIPTS_API virtual ~ScriptComponent();

	GAMESCRIPTS_API bool HasStarted()const;
	GAMESCRIPTS_API void SetStarted(bool started);

	// ----------------------------------------------------
	// ライフサイクル関数 (ユーザーがオーバーライドする場所)
	// ----------------------------------------------------

	//初期化処理:Startの前に呼ばれるものとして宣言
	GAMESCRIPTS_API virtual void Awake();

	//初期化処理:最初のUpdate()が呼ばれる前に一度だけ呼ばれる
	GAMESCRIPTS_API virtual void Start();

	//毎フレームの更新処理
	GAMESCRIPTS_API virtual void Update(float deltaTime)override;

	//物理更新処理
	GAMESCRIPTS_API virtual void FixedUpdate(float deltaTime)override;

};

