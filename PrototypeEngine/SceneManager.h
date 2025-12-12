#pragma once
#include "Typedefs.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class BaseScene;
class ActorObject;

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

	//シーンのロード処理(GUI用)
	static void											LoadSceneGUI(const string& filePath);
	//シーンの削除
	static void											ReleaseAllScenes();
	//現在のシーンの取得
	static BaseScene*									GetNowScene() { return mNowScene; }
	static void											SetNowScene(BaseScene* scene) { mNowScene = scene; }
	//ゲーム用の関数
	static void											ChangeScene();
	static void											GamePlayEndInitilaizeScene();
	//ロードフラグの取得
	static bool											IsLoading() { return loading; }
	//現在のシーンの数値のGetter
	static int											GetNowSceneIndex() { return mNowSceneIndex; }
	//エンジン起動時にロードすべきシーンファイルのパス
	static string										mDefaultSceneFilePath;
	//現在エディタで開いているシーンのファイルパスを設定する関数
	static void											SetCurrentEditorSceneFilePath(const string& path);
	//現在のアクティブシーンのアクターリストを返す
	static const	vector<ActorObject*>&				GetAllActorsInCurrentScene();
};

