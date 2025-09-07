#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "SplineCamera.h"
#include "MoveComponent.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分
//スプラインカメラに使うオブジェクトクラス
//現在は未使用ですが今後使えるようにするためにおいています。
class SplineActor : public ActorObject
{
private:
	SplineCamera*			mCameraComp;
public:
							SplineActor();

	void					ActorInput(const struct InputState& keys) override;

	void					RestartSpline();
};

