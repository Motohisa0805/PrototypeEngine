#include "SplineCamera.h"

Vector3 Spline::Compute(size_t startIdx, float t) const
{
	// startIdxが範囲外であるか確認してください
	if (startIdx >= mControlPoints.size())
	{
		return mControlPoints.back();
	}
	else if (startIdx == 0)
	{
		return mControlPoints[startIdx];
	}
	else if (startIdx + 2 >= mControlPoints.size())
	{
		return mControlPoints[startIdx];
	}

	// p0からp3を取得する
	Vector3 p0 = mControlPoints[startIdx - 1];
	Vector3 p1 = mControlPoints[startIdx];
	Vector3 p2 = mControlPoints[startIdx + 1];
	Vector3 p3 = mControlPoints[startIdx + 2];
	// キャトマル・ロム方程式に従って位置を計算する
	Vector3 position = 0.5f * ((2.0f * p1) + (-1.0f * p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
		(-1.0f * p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
	return position;
}

SplineCamera::SplineCamera(ActorObject* owner)
	:BaseCamera(owner)
	, mIndex(1)
	, mT(0.0f)
	, mSpeed(0.5f)
	, mPaused(true)
{
}

void SplineCamera::Update(float deltaTime)
{
	BaseCamera::Update(deltaTime);
	// t値を更新する
	if (!mPaused)
	{
		mT += mSpeed * deltaTime;
		// 必要であれば、次のコントロールポイントに進む。
		// スピードが速すぎて一フレーム内に複数のコントロールポイントを飛び越えてしまうことはないと仮定。
		if (mT >= 1.0f)
		{
			// 進むためのポイントがあることを確認。
			if (mIndex < mPath.GetNumPoints() - 3)
			{
				mIndex++;
				mT = mT - 1.0f;
			}
			else
			{
				//一時停止。
				mPaused = true;
			}
		}
	}

	// カメラの位置は現在のt/インデックスにおけるスプラインです。
	Vector3 cameraPos = mPath.Compute(mIndex, mT);
	// ターゲットポイントはスプラインの前にわずかに小さなデルタです。
	Vector3 target = mPath.Compute(mIndex, mT + 0.01f);
	// スプラインが逆さまにならないと仮定。
	const Vector3 up = Vector3::UnitZ;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}

void SplineCamera::Restart()
{
	mIndex = 1;
	mT = 0.0f;
	mPaused = false;
}