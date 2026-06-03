#pragma once
#include "Actor.h"
#include "UIActor.h"
#include "Canvas.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "EditorSettingsManager.h"

//コマンドパターンの基底クラス
class ICommand
{
public:
	virtual ~ICommand() = default;
	//コマンドの実行
	virtual void Execute() = 0;
	//コマンドの実行（履歴に残さないバージョン）
	virtual void NoHistoryExecute() = 0;
	//コマンドの元に戻す
	virtual void Undo() = 0;
	//コマンドのやり直し
	virtual void Redo() = 0;
};