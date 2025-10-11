#pragma once
#include "StandardLibrary.h"
#include "SceneSerializer.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class BaseScene;

//シーンの管理、ロード処理を行うクラス
class SceneManager
{
private:

	//現在のシーン
	static BaseScene*									mNowScene;
	//ロードフラグ
	static bool											loading;
	//現在のシーンの数値
	static int											mNowSceneIndex;
	static string										mNextSceneFilePath;
public:
	//シーンの初期化処理
	static bool											InitializeScenes();

	//シーンのロード処理
	static void											LoadSceneFromFile(const string& filePath);
	// 現在編集中/実行中のシーンが、ファイルからロードされたものかを示すフラグ（任意）
	static bool											mIsFileLoadedScene;
	//シーンの削除
	static void											ReleaseAllScenes();
	//現在のシーンの取得
	static BaseScene*									GetNowScene() { return mNowScene; }
	static void											SetNowScene(BaseScene* scene) { mNowScene = scene; }
	static void											ChangeScene();
	static void											PlayEndInitilaizeScene();
	//ロードフラグの取得
	static bool											IsLoading() { return loading; }
	//ロードフラグを解除
	static void											DisabledLoading() { loading = false; }
	//現在のシーンの数値のGetter
	static int											GetNowSceneIndex() { return mNowSceneIndex; }
	//エンジン起動時にロードすべきシーンファイルのパス
	static string										mDefaultSceneFilePath;
	//現在エディタで開いているシーンのファイルパスを設定する関数
	static void											SetCurrentEditorSceneFilePath(const string& path);
};

