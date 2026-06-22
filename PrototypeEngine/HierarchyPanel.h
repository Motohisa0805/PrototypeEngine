#pragma once
#include "EditorWindow.h"

class ActorObject;
class UIActorObject;
//ゲームのシーン内のオブジェクトを描画する予定のクラス
//本格的な描画処理は未実装
class HierarchyPanel : public EditorWindow
{
public:
				HierarchyPanel(Renderer* renderer);
				~HierarchyPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height)override;

	void		DrawActorNode(ActorObject* actor);

	void		DrawUIActorNode(UIActorObject* actor);

	// 右クリックメニュー
	bool		RightClickMenu();

	static void	EditorCommandPopupMenu();

	void		ClearPointer()override;
};