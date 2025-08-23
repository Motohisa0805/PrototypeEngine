#pragma once
#include "Math.h"
#include "StandardLibrary.h"

//画面の描画に関係する変数を管理しているクラス
//画面サイズ、カメラの目、ターゲット、方向、視野角、最低・最大描画距離
class WindowRenderProperty
{
private:
	//画面サイズ(横)
	static float		mWidth;
	//画面サイズ(縦)
	static float		mHeight;

	//ビュー投影行列(カメラの目)
	static Vector3		mViewEye;
	//ビュー投影行列(カメラターゲット)
	static Vector3		mViewTarget;
	//ビュー投影行列(カメラ方向)
	static Vector3		mViewUp;
	//視野角
	static float		mFieldOfView;
	//カメラの最低描画距離
	static float		mCameraNear;
	//カメラの最大描画距離
	static float		mCameraFar;
public:
	static float		GetWidth() { return mWidth; }

	static float		GetHeight() { return mHeight; }

	static Vector3		GetViewEye() { return mViewEye; }

	static Vector3		GetViewTarget() { return mViewTarget; }

	static Vector3		GetViewUp() { return mViewUp; }

	static float		GetFieldOfView() { return mFieldOfView; }

	static float		GetCameraNear() { return mCameraNear; }

	static float		GetCameraFar() { return mCameraFar; }

	static void			SetWindowScale(float width, float height);

	static void			SetViewEye(Vector3 eye);

	static void			SetViewTarget(Vector3 target);

	static void			SetViewUp(Vector3 up);

	static void			SetFieldOfView(float fieldOfView);

	static void			SetCameraNear(float cameraNear);

	static void			SetCameraFar(float cameraFar);
};

