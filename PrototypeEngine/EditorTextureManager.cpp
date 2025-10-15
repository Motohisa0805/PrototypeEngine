#include "EditorTextureManager.h"

EditorTextureManager::EditorTextureManager()
{
	// メンバー変数の初期化、テクスチャのロードなどはここでは行わないのが一般的
	// (通常、ロードはAllLoad()で行う)

	mPlayButtonTexture = nullptr;
	mPauseButtonTexture = nullptr;
	mStopButtonTexture = nullptr;
	mFrameByFrameButtonTexture = nullptr;
}

void EditorTextureManager::AllLoad()
{
	mPlayButtonTexture = new Texture();
	if (!mPlayButtonTexture->Load("Assets/Editor/PlayButton.png"))
	{
		Debug::ErrorLog("Failed to load play button texture");
		delete mPlayButtonTexture;
		mPlayButtonTexture = nullptr;
	}
	mStopButtonTexture = new Texture();
	if (!mStopButtonTexture->Load("Assets/Editor/StopButton.png"))
	{
		Debug::ErrorLog("Failed to load stop button texture");
		delete mStopButtonTexture;
		mStopButtonTexture = nullptr;
	}
	mPauseButtonTexture = new Texture();
	if (!mPauseButtonTexture->Load("Assets/Editor/PauseButton.png"))
	{
		Debug::ErrorLog("Failed to load pause button texture");
		delete mPauseButtonTexture;
		mPauseButtonTexture = nullptr;
	}
	mFrameByFrameButtonTexture = new Texture();
	if (!mFrameByFrameButtonTexture->Load("Assets/Editor/FrameByFrame.png"))
	{
		Debug::ErrorLog("Failed to load frame by frame button texture");
		delete mFrameByFrameButtonTexture;
		mFrameByFrameButtonTexture = nullptr;
	}
}

void EditorTextureManager::AllRelease()
{
	if (mPlayButtonTexture)
	{
		mPlayButtonTexture->Unload();
		delete mPlayButtonTexture;
		mPlayButtonTexture = nullptr;
	}
	if (mPauseButtonTexture)
	{
		mPauseButtonTexture->Unload();
		delete mPauseButtonTexture;
		mPauseButtonTexture = nullptr;
	}
	if (mStopButtonTexture)
	{
		mStopButtonTexture->Unload();
		delete mStopButtonTexture;
		mStopButtonTexture = nullptr;
	}
	if (mFrameByFrameButtonTexture)
	{
		mFrameByFrameButtonTexture->Unload();
		delete mFrameByFrameButtonTexture;
		mFrameByFrameButtonTexture = nullptr;
	}
}
