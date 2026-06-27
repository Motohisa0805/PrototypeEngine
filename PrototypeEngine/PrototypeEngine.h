#pragma once
#include "StandardLibrary.h"
#include "DebugManager.h"
#include "SDL3.h"
#include "Random.h"
#include "Math.h"
#include "Time.h"
#include "StringConvertOperation.h"
#include "CallBackManager.h"
#include "VertexArray.h"
#include "Assimp.h"
#include "FilePath.h"
#include "WindowRenderProperty.h"
#include "AudioBankProperty.h"
#include "Typedefs.h"
#include "PhysWorld.h"
#include "AudioSystem.h"
#include "SoundEventClip.h"
#include "SceneManager.h"
#include "InputSystem.h"
#include "DebugGrid.h"

//***ゲームシーンで必要なオブジェクトや画像の情報をまとめているファイル
//Actor
#include "Actor.h"
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
#include "Canvas.h"
//ライト関係のインクルード
#include "LocalLight.h"
// Skybox関係のインクルード
#include "SkyBoxRenderer.h"