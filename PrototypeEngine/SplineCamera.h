#pragma once
#include "BaseCamera.h"
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍部分のファイル
struct Spline
{
	// スプラインの制御点（
	// セグメント内の点の数をnとすると、n+2のポイントが必要）
	vector<Vector3> mControlPoints;
	// 開始インデックスがP1であるスプラインセグメントを考慮して、
	// t値に基づいて位置を計算します。
	Vector3			Compute(size_t startIdx, float t) const;
	// 制御点の数を返します。
	size_t			GetNumPoints() const { return mControlPoints.size(); }
};

class SplineCamera : public BaseCamera
{
private:
	// スプラインパスに沿ってカメラが移動します
	Spline	mPath;
	// 現在の制御ポイントインデックスとt
	size_t	mIndex;
	float	mT;
	// 量の変化/秒
	float	mSpeed;
	// カメラを経路に沿って移動させるかどうか
	bool	mPaused;
public:
			SplineCamera(ActorObject* owner);

	void	Update(float deltaTime) override;
	// スプラインを再起動してください
	void	Restart();

	void	SetSpeed(float speed) { mSpeed = speed; }
	void	SetSpline(const Spline& spline) { mPath = spline; }
	void	SetPaused(bool pause) { mPaused = pause; }
};