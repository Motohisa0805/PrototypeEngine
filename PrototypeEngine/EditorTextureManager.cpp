#include "EditorTextureManager.h"
#include "DebugManager.h"
#include "Texture.h"
#include "Renderer.h"

EditorTextureManager::EditorTextureManager()
	:mPlayButtonTexture(nullptr)
	,mPauseButtonTexture(nullptr)
	,mStopButtonTexture(nullptr)
	,mFrameByFrameButtonTexture(nullptr)
	,mTestImage(nullptr)
	,mNoImageTexture(nullptr)
	,mFBXTexture(nullptr)
	,mHFileTexture(nullptr)
	,mCPPFileTexture(nullptr)
{
	// メンバー変数の初期化、テクスチャのロードなどはここでは行わないのが一般的
	// (通常、ロードはAllLoad()で行う)

	mPlayButtonTexture = nullptr;
	mPauseButtonTexture = nullptr;
	mStopButtonTexture = nullptr;
	mFrameByFrameButtonTexture = nullptr;
	mTestImage = nullptr;
	mNoImageTexture = nullptr;
	mFBXTexture = nullptr;
	mHFileTexture = nullptr;
	mCPPFileTexture = nullptr;
}

void EditorTextureManager::AllLoad()
{
	mPlayButtonTexture = new Texture();
	if (!mPlayButtonTexture->Load("Editor/PlayButton.png")){
		Debug::ErrorLog("Failed to load play button texture");
		delete mPlayButtonTexture;
		mPlayButtonTexture = nullptr;
	}
	mStopButtonTexture = new Texture();
	if (!mStopButtonTexture->Load("Editor/StopButton.png")){
		Debug::ErrorLog("Failed to load stop button texture");
		delete mStopButtonTexture;
		mStopButtonTexture = nullptr;
	}
	mPauseButtonTexture = new Texture();
	if (!mPauseButtonTexture->Load("Editor/PauseButton.png")){
		Debug::ErrorLog("Failed to load pause button texture");
		delete mPauseButtonTexture;
		mPauseButtonTexture = nullptr;
	}
	mFrameByFrameButtonTexture = new Texture();
	if (!mFrameByFrameButtonTexture->Load("Editor/FrameByFrame.png")){
		Debug::ErrorLog("Failed to load frame by frame button texture");
		delete mFrameByFrameButtonTexture;
		mFrameByFrameButtonTexture = nullptr;
	}
	mTestImage = new Texture();
	if (!mTestImage->Load("Editor/TestImage.png")){
		Debug::ErrorLog("Failed to load TestImage texture");
		delete mTestImage;
		mTestImage = nullptr;
	}
	mNoImageTexture = new Texture();
	if (!mNoImageTexture->Load("Editor/NoImageIcon.png")){
		Debug::ErrorLog("Failed to load NoImage texture");
		delete mNoImageTexture;
		mNoImageTexture = nullptr;
	}
	mFolderTexture = new Texture();
	if (!mFolderTexture->Load("Editor/FolderIcon.png")){
		Debug::ErrorLog("Failed to load folder texture");
		delete mFolderTexture;
		mFolderTexture = nullptr;
	}
	mSceneTexture = new Texture();
	if (!mSceneTexture->Load("Editor/SceneIcon.png")) {
		Debug::ErrorLog("Failed to load Scene texture");
		delete mSceneTexture;
		mSceneTexture = nullptr;
	}
	mFBXTexture = new Texture();
	if (!mFBXTexture->Load("Editor/FBXFileIcon.png")){
		Debug::ErrorLog("Failed to load FBX texture");
		delete mFBXTexture;
		mFBXTexture = nullptr;
	}
	mHFileTexture = new Texture();
	if (!mHFileTexture->Load("Editor/HFileIcon.png")){
		Debug::ErrorLog("Failed to load H file texture");
		delete mHFileTexture;
		mHFileTexture = nullptr;
	}
	mCPPFileTexture = new Texture();
	if (!mCPPFileTexture->Load("Editor/CPPFileIcon.png")){
		Debug::ErrorLog("Failed to load CPP file texture");
		delete mCPPFileTexture;
		mCPPFileTexture = nullptr;
	}
	mTTFFontFileTexture = new Texture();
	if (!mTTFFontFileTexture->Load("Editor/FontIcon.png")){
		Debug::ErrorLog("Failed to load TTF file texture");
		delete mTTFFontFileTexture;
		mTTFFontFileTexture = nullptr;
	}

	// 拡張子とテクスチャのマップを初期化
	mExtensionTextureMap[""] = mFolderTexture; 
	mExtensionTextureMap[".fbm"] = mFolderTexture; 
	mExtensionTextureMap[".png"] = mTestImage; 
	mExtensionTextureMap[".json"] = mSceneTexture;
	mExtensionTextureMap[".fbx"] = mFBXTexture; 
	mExtensionTextureMap[".h"] = mHFileTexture; 
	mExtensionTextureMap[".cpp"] = mCPPFileTexture; 
	mExtensionTextureMap[".bank"] = mTestImage; 
	mExtensionTextureMap[".ttf"] = mTTFFontFileTexture; 
}

void EditorTextureManager::AllRelease()
{
	if (mPlayButtonTexture){
		mPlayButtonTexture->Unload();
		delete mPlayButtonTexture;
		mPlayButtonTexture = nullptr;
	}
	if (mPauseButtonTexture){
		mPauseButtonTexture->Unload();
		delete mPauseButtonTexture;
		mPauseButtonTexture = nullptr;
	}
	if (mStopButtonTexture){
		mStopButtonTexture->Unload();
		delete mStopButtonTexture;
		mStopButtonTexture = nullptr;
	}
	if (mFrameByFrameButtonTexture){
		mFrameByFrameButtonTexture->Unload();
		delete mFrameByFrameButtonTexture;
		mFrameByFrameButtonTexture = nullptr;
	}
	if (mTestImage){
		mTestImage->Unload();
		delete mTestImage;
		mTestImage = nullptr;
	}
	if (mNoImageTexture){
		mNoImageTexture->Unload();
		delete mNoImageTexture;
		mNoImageTexture = nullptr;
	}
	if(mFolderTexture){
		mFolderTexture->Unload();
		delete mFolderTexture;
		mFolderTexture = nullptr;
	}
	if (mSceneTexture) {
		mSceneTexture->Unload();
		delete mSceneTexture;
		mSceneTexture = nullptr;
	}
	if (mFBXTexture){
		mFBXTexture->Unload();
		delete mFBXTexture;
		mFBXTexture = nullptr;
	}
	if (mHFileTexture){
		mHFileTexture->Unload();
		delete mHFileTexture;
		mHFileTexture = nullptr;
	}
	if (mCPPFileTexture){
		mCPPFileTexture->Unload();
		delete mCPPFileTexture;
		mCPPFileTexture = nullptr;
	}
	if (mTTFFontFileTexture){
		mTTFFontFileTexture->Unload();
		delete mTTFFontFileTexture;
		mTTFFontFileTexture = nullptr;
	}

	// 拡張子とテクスチャのマップもクリア
	mExtensionTextureMap.clear();

	for (auto t : mThumbnailCacheMap)
	{
		if (t.second)
		{
			t.second->Unload();
			delete t.second;
			t.second = nullptr;
		}
	}
	mThumbnailCacheMap.clear();
}

Texture* EditorTextureManager::GetFileExtensionTexture(const std::string& extension) const
{
	Texture* texture = nullptr;
	// ここで拡張子に応じたテクスチャを返す処理を実装
	if (mExtensionTextureMap.find(extension) != mExtensionTextureMap.end()) {
		texture = mExtensionTextureMap.at(extension); // 例として、拡張子に対応するテクスチャをマップから取得
	}
	else {
		texture = mNoImageTexture; // 例として、デフォルトのテクスチャを返す
	}
	return texture;
}

Texture* EditorTextureManager::GetFileIconTexture(const std::string& filePath, const std::string& extension)
{
	//画像ファイル読み込み
	if (extension == ".png" || extension == ".jpg") {
		// 既にそのファイルのサムネイルがロード済み（キャッシュにある）ならそれを返す
		if (mThumbnailCacheMap.find(filePath) != mThumbnailCacheMap.end()) {
			return mThumbnailCacheMap.at(filePath);
		}

		// キャッシュにない場合：新しくテクスチャを作ってロードする
		Texture* thumbnail = new Texture();

		// 【簡易実装】とりあえず元の画像をそのままロードする場合
		// (TODO : 事前にリサイズされた軽量なキャッシュ画像をロードする)
		if (thumbnail->Load(filePath.c_str())) {
			mThumbnailCacheMap[filePath] = thumbnail;
			return thumbnail;
		}
		else {
			delete thumbnail;
			return mNoImageTexture; // ロード失敗時はデフォルト
		}
	}

	// 2. 画像以外は、従来の拡張子マップから固定アイコンを返す
	if (mExtensionTextureMap.find(extension) != mExtensionTextureMap.end()) {
		return mExtensionTextureMap.at(extension);
	}

	return mNoImageTexture;
}
