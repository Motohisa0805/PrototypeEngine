#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "OrbitCamera.h"
#include "MoveComponent.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分
//軌道カメラに使うオブジェクトのクラス
//今後使うため残しています。
class OrbitActor : public ActorObject
{
private:
	OrbitCamera*			mCameraComp;

	MeshRenderer*			mMeshComp;
public:
							OrbitActor();

	void					ActorInput(const struct InputState& keys) override;

	void					SetVisible(bool visible);
};

