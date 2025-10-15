#pragma once
#include "SDL3.h"
#include "Renderer.h"

//前方宣言
class GameWinMain;

//ゲーム全体の処理を行うクラス
//ここにゲームに必要な処理を書いていく想定で制作しました。
class GameApp
{
private:
	//描画管理クラス
	GameWinMain*				mWinMain;
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
};