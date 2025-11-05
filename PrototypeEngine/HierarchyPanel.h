#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

class ActorObject;
//ゲームのシーン内のオブジェクトを描画する予定のクラス
//本格的な描画処理は未実装
class HierarchyPanel : public GUIPanel
{
private:
	//選択中のアクターを保持するポインター
	ActorObject*			mSelectedActor;

	string					mRenameInputBuffer;
	vector<RenameRequest>	mRenameQueue;

	bool					mRenaming;
public:
	const char* GetName()override { return "Hierarchy"; }
	HierarchyPanel(class Renderer* renderer);
	~HierarchyPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;

	void		DrawActorNode(ActorObject* actor);

	void		ClearPointer()override;

	//保留中の削除、リネーム、ドラッグ＆ドロップの処理
	void		ProcessPendingOperations();

	//外部から選択中のActorを取得
	ActorObject* GetSelectedActor() const { return mSelectedActor; }
};

