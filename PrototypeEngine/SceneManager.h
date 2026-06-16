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
	static BaseScene*									mCurrentRunScene;
	//ロードフラグ
	static bool											mIsLoading;

	static string										mNextSceneFilePath;
public:
	//シーンの初期化処理
	static bool											InitializeScenes();

	//シーンのロード処理(GUI用)
	static void											LoadSceneGUI(const string& filePath);
	//シーンの削除
	static void											ReleaseAllScenes();
	//現在のシーンの取得
	static BaseScene*									GetCurrentRunScene() { return mCurrentRunScene; }
	static void											SetCurrentRunScene(BaseScene* scene) { mCurrentRunScene = scene; }
	//ゲーム用の関数
	static void											ChangeScene();
	static void											GamePlayEndInitilaizeScene();
	//ロードフラグの取得
	static bool											IsLoading() { return mIsLoading; }
	//現在のアクティブシーンのアクターリストを返す
	static const	vector<ActorObject*>&				GetAllActorsInCurrentScene();
};

