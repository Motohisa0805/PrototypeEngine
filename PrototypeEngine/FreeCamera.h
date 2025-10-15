#pragma once
#include "BaseCamera.h"
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//指定した方向を映すカメラ
//FreeCameraは、ユーザーが自由にカメラの位置と向きを制御できるカメラです。
//Componentを継承しているので、ActorObjectにアタッチして使用します。
class FreeCamera : public BaseCamera
{
private:
	bool	isUpdatePosition;

	// カメラの上方向ベクトル。
	Vector3		mUp;
public:
			FreeCamera(ActorObject* owner);

	void    Initiailze();

	void	Update(float deltaTime) override;

	void    SetCameraPosition(const Vector3& eye, const Vector3& target);

	void	DrawGUI()override;


	void	MoveCameraView();
};

