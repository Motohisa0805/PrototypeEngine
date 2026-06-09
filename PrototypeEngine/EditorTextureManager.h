#pragma once
#include "Typedefs.h"

//前方宣言
class Renderer;
class Texture;
//エンジンで使用するGUI画像を読み込み、保存、解放するクラス
class EditorTextureManager
{
private:
	EditorTextureManager();

	Texture*									mPlayButtonTexture;
	Texture*									mPauseButtonTexture;
	Texture*									mStopButtonTexture;
	Texture*									mFrameByFrameButtonTexture;

	Texture*									mTestImage;

	Texture*									mNoImageTexture; // ファイルが見つからない場合のテクスチャ
	Texture*									mFolderTexture; // フォルダアイコン用のテクスチャ
	Texture*									mSceneTexture; // シーンアイコン用のテクスチャ
	Texture*									mFBXTexture; // FBXファイル用のテクスチャ
	Texture*									mHFileTexture; // .hファイル用のテクスチャ
	Texture*									mCPPFileTexture; // .cppファイル用のテクスチャ
	Texture*									mTTFFontFileTexture; // .ttfファイル用のテクスチャ
	// 拡張子とテクスチャのマップ
	std::map<std::string, Texture*>				mExtensionTextureMap; 
	// 固定アイコン用のマップとは別に、ファイルごとのサムネイルを管理するマップを用意
	std::unordered_map<std::string, Texture*>	mThumbnailCacheMap;

	Texture*									mOpenGLLogo;

	Texture*									mSDL3Logo;
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
	void		AllLoad();
	void		AllRelease();

	Texture*	GetFileExtensionTexture(const std::string& extension) const;

	// 関数を拡張：拡張子だけでなく「ファイルパス」も受け取れるようにする
	Texture*	GetFileIconTexture(const std::string& filePath, const std::string& extension);

	Texture*	GetPlayButtonTexture()const { return mPlayButtonTexture; }
	Texture*	GetPauseButtonTexture()const { return mPauseButtonTexture; }
	Texture*	GetStopButtonTexture()const { return mStopButtonTexture; }
	Texture*	GetFrameByFrameButtonTexture()const { return mFrameByFrameButtonTexture; }
	Texture*	GetTestImage()const { return mTestImage; }

	Texture*	GetOpenGLLogo()const { return mOpenGLLogo; }
	Texture*	GetSDL3Logo()const { return mSDL3Logo; }
};

