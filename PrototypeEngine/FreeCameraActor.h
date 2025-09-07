#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "FreeCamera.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

// フリーカメラを使用するためのアクター
// Unityのようにデフォルトで作成可能オブジェクト
class FreeCameraActor : public ActorObject
{
private:
	FreeCamera*			mCameraComp;
public:
						FreeCameraActor();

	FreeCamera*			GetCamera() { return mCameraComp; }
};

