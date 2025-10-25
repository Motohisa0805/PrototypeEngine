#pragma once
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/
//前方宣言
class FreeCamera;

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

