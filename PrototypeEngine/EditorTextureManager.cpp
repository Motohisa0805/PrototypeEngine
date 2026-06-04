#include "EditorTextureManager.h"
#include "DebugManager.h"
#include "Texture.h"
#include "Renderer.h"

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
	if (!mPlayButtonTexture->Load("Editor/PlayButton.png"))
	{
		Debug::ErrorLog("Failed to load play button texture");
		delete mPlayButtonTexture;
		mPlayButtonTexture = nullptr;
	}
	mStopButtonTexture = new Texture();
	if (!mStopButtonTexture->Load("Editor/StopButton.png"))
	{
		Debug::ErrorLog("Failed to load stop button texture");
		delete mStopButtonTexture;
		mStopButtonTexture = nullptr;
	}
	mPauseButtonTexture = new Texture();
	if (!mPauseButtonTexture->Load("Editor/PauseButton.png"))
	{
		Debug::ErrorLog("Failed to load pause button texture");
		delete mPauseButtonTexture;
		mPauseButtonTexture = nullptr;
	}
	mFrameByFrameButtonTexture = new Texture();
	if (!mFrameByFrameButtonTexture->Load("Editor/FrameByFrame.png"))
	{
		Debug::ErrorLog("Failed to load frame by frame button texture");
		delete mFrameByFrameButtonTexture;
		mFrameByFrameButtonTexture = nullptr;
	}
	mTestImage = new Texture();
	if (!mTestImage->Load("Editor/TestImage.png"))
	{
		Debug::ErrorLog("Failed to load TestImage texture");
		delete mTestImage;
		mTestImage = nullptr;
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
	if (mTestImage)
	{
		mTestImage->Unload();
		delete mTestImage;
		mTestImage = nullptr;
	}
}
