#pragma once
#include "StandardLibrary.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class BaseScene;

//シーンの管理、ロード処理を行うクラス
class SceneManager
{
private:
	//ロード対象のシーンリスト
	static std::unordered_map<int, BaseScene*>			mScenes;
	//現在のシーン
	static BaseScene*									mNowScene;
	static BaseScene*									mNextScene;
	//ロードフラグ
	static bool											loading;
	//現在のシーンの数値
	static int											mNowSceneIndex;
public:
	//シーンの初期化処理
	static bool											InitializeScenes();

	//シーンのロード処理
	static void											LoadScene(int index);
	//シーンリストに追加
	static void											AddSceneList(BaseScene* scene);
	//シーンの削除
	static void											ReleaseAllScenes();
	//現在のシーンの取得
	static BaseScene*									GetNowScene() { return mNowScene; }
	static void											ChangeScene();
	//ロードフラグの取得
	static bool											IsLoading() { return loading; }
	//ロードフラグを解除
	static void											DisabledLoading() { loading = false; }
	//現在のシーンの数値のGetter
	static int											GetNowSceneIndex() { return mNowSceneIndex; }
};

