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

	static filesystem::path mTempParentPath;
	static filesystem::path mTempPath;
public:
	//編集したシーンデータが入った復元データ
	static filesystem::path GetTempPath() { return mTempPath; }
	// メモリ上のアクターリストを受け取り、指定パスにシーンを保存する
	static bool SaveScene(const filesystem::path& filePath, BaseScene* scene);
	//GUIで空のシーンを生成する関数
	static bool SaveEmptyScene(const filesystem::path& filePath);
	//シーンを読み込む関数
	static BaseScene* LoadScene(const string& filePath);
	//編集用データの保存関数
	static void WriteEditorData(const filesystem::path& filePath, BaseScene* scene);
	//編集用データの解放関数
	static void RelaseEditorData();
};

