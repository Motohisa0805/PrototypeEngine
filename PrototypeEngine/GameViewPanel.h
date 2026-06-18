#pragma once
#include "GUIEditorManager.h"
#include "EditorWindow.h"

//GUIのゲームパネルの処理をまとめたクラス
class GameViewPanel : public EditorWindow
{
private:
	bool		mIsDebugStatesFrag;
public:
	GameViewPanel(class Renderer* renderer);

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width,float height)override;

	bool		IsDebugStatesFrag() { return mIsDebugStatesFrag; }
};
