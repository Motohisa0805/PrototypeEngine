#pragma once
#include "EditorWindow.h"

class GUIMainMenu : public EditorWindow
{
private:

public:
	GUIMainMenu(Renderer* renderer);
	~GUIMainMenu();
	//GUIの初期化
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	//GUIの描画
	void Draw(float width, float height)override;
	//ファイルメニューの描画
	void FileMenuDraw();
	//エディターメニューの描画
	void EditorMenuDraw();
	//アセットメニューの描画
	void AssetMenuDraw();
	//ゲームオブジェクトメニューの描画
	void GameObjectMenuDraw();
	//コンポーネントメニューの描画
	void ComponentMenuDraw();
	//表示メニューの描画
	void WindowMenuDraw();
	//ヘルプメニューの描画
	void HelpMenuDraw();

	void SetPopupColorTheme()override;
	void ResetPopupColorTheme()override;
};

