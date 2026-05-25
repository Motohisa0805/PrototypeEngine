#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"
#include "SelectionManager.h"
#include "CommandManager.h"

class ActorObject;
class UIActorObject;
//ゲームのシーン内のオブジェクトを描画する予定のクラス
//本格的な描画処理は未実装
class HierarchyPanel : public GUIPanel
{
private:
	string					mRenameInputBuffer;

	bool					mRenaming;
public:
	const char* GetName()override { return "Hierarchy"; }
	HierarchyPanel(class Renderer* renderer);
	~HierarchyPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;

	void		DrawActorNode(ActorObject* actor);

	// 右クリックメニュー
	bool		RightClickMenu();

	void		ClearPointer()override;
};