#pragma once

/*
* ===エンジン内部処理/Engine internal processing===
*/

//***ゲームシーンで必要なオブジェクトや画像の情報をまとめているファイル
//Actor
#include "Actor.h"
#include "MeshActor.h"
#include "SkeletonMeshActor.h"
//プロジェクトにデフォルトで入っているオブジェクトクラスのインクルード
#include "PlaneActor.h"
#include "CubeActor.h"
#include "SphereActor.h"
#include "CapsuleActor.h"
//Actorを継承したオブジェクトクラスのインクルード
#include "FPSActor.h"
#include "TPSPlayer.h"
#include "BallActor.h"
//カメラの基底クラスのインクルード
#include "BaseCamera.h"
//スケルタルメッシュ関係のインクルード
#include "MeshRenderer.h"
#include "Skeleton.h"
//アニメーション関係のインクルード
#include "Animation.h"
#include "Animator.h"
//デフォルトでプロジェクトにある画像、テキスト描画関係のインクルード
#include "Image.h"
#include "Font.h"
#include "Text.h"
//UI関係のインクルード
#include "PauseMenu.h"
#include "Canvas.h"
//ライト関係のインクルード
#include "DirectionalLightActor.h"
#include "PointLightComponent.h"
// Skybox関係のインクルード
#include "SkyBoxRenderer.h"