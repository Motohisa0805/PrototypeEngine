#pragma once
#include "Texture.h"
#include "Renderer.h"
//前方宣言
class Renderer;
//エンジンで使用するGUI画像を読み込み、保存、解放するクラス
class EditorTextureManager
{
private:
	EditorTextureManager();

	Texture* mPlayButtonTexture;
	Texture* mPauseButtonTexture;
	Texture* mStopButtonTexture;
	Texture* mFrameByFrameButtonTexture;
public:
	static EditorTextureManager& GetInstance()
	{
		//静的変数としてインスタンスを定義
		static EditorTextureManager instance;
		return instance;
	}
	//コピーコンストラクタと代入演算子を削除
	EditorTextureManager(const EditorTextureManager&) = delete;
	EditorTextureManager& operator = (const EditorTextureManager&) = delete;
	void AllLoad();
	void AllRelease();

	Texture* GetPlayButtonTexture()const { return mPlayButtonTexture; }
	Texture* GetPauseButtonTexture()const { return mPauseButtonTexture; }
	Texture* GetStopButtonTexture()const { return mStopButtonTexture; }
	Texture* GetFrameByFrameButtonTexture()const { return mFrameByFrameButtonTexture; }
};

