#pragma once
#include "SDL3.h"
#include "Renderer.h"

//ゲーム全体の処理を行うクラス
//ここにゲームに必要な処理を書いていく想定で制作しました。
class GameApp
{
private:
	//現在の有効なシーン
	static class BaseScene*		mActiveScene;
	class TitleScene*			mTitleScene;
	//シーン01
	class DebugScene01*			mDebugScene01;
	//シーン02
	class DebugScene02*			mDebugScene02;
	//描画管理クラス
	class GameWinMain*			mWinMain;
public:
								GameApp(class GameWinMain* main);
	//初期化
	bool						Initialize();
	//入力処理
	bool						ProcessInput();
	//シーンのロード処理
	bool						LoadUpdate();
	//更新処理
	bool						Update();
	//解放
	bool						Release();
	//現在有効なシーンをstaticで取得
	static class BaseScene*		GetActiveScene() { return mActiveScene; }
};