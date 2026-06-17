#pragma once
#include "StandardLibrary.h"
#include "Typedefs.h"
#include "FilePath.h"

class BaseScene;
class ActorObject;
// Sceneのシリアライズ/デシリアライズを行うクラス
// JSON形式でシーンデータを保存・読み込みする
class SceneSerializer
{
private:

	static filesystem::path mTempEditingDirectoryPath;
	static filesystem::path mTempEditingPath;
public:
	//編集したシーンデータが入った復元データ
	static filesystem::path		GetTempEditingPath() { return mTempEditingPath; }
	// メモリ上のアクターリストを受け取り、指定パスにシーンを保存する
	static bool					SaveRunScene(const filesystem::path& filePath, BaseScene* scene);
	//GUIで空のシーンを生成する関数
	static bool					CreateEmptyScene(const filesystem::path& filePath);
	//シーンを読み込む関数
	static BaseScene*			LoadScene(const string& filePath,bool isWriteTempData = false);

	static void					RenameRunScene(const filesystem::path& filePath,const string& newFileName);

	//編集用データの保存関数
	static void					WriteEditingSceneData(const filesystem::path& filePath, BaseScene* scene);
	//編集用データの解放関数
	static void					RelaseEditorData();
};

