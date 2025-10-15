#pragma once
#include "StandardLibrary.h"
#include "Typedefs.h"

class BaseScene;
class ActorObject;

class SceneSerializer
{
private:
public:
	// メモリ上のアクターリストを受け取り、指定パスにシーンを保存する
	static bool SaveScene(const filesystem::path& filePath, BaseScene* scene);
	static bool SaveEmptyScene(const filesystem::path& filePath);
	static BaseScene* LoadScene(const string& filePath);
};

