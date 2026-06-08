#pragma once
#include "EditorTextureManager.h"
#include "Texture.h"
#include "Renderer.h"
#include "ImGuiHelper.h"

//前方宣言
class Renderer;
//GUIのパネルの継承元クラス
//名前、マウスカーソルがパネル内にあるかどうかなどの処理を行っています。
class EditorWindow
{
protected:
	//ウィンドウのID
	string						mID;
	//描画情報
	Renderer*					mRenderer;
	//カーソル判定
	bool						isMouseHovered;
	//左上を0基準の矩形情報と同じ
	float						mWidthPos;
	float						mHeightPos;
	float						mWidthSize;
	float						mHeightSize;
	//GUIのレイアウトを初期位置に戻すフラグ
	bool						isResetLayout;
	//表示/非表示フラグ
	bool						mIsShow;

	vector<EditorWindow*>		mChildren;
public:
								EditorWindow(Renderer* renderer);
	virtual						~EditorWindow(){
		mChildren.clear();
	}

	virtual void				Draw(float width, float height);
	
	void						AddEditorWindow(EditorWindow* window);

	void						RemoveEditorWindow(EditorWindow* window);

	EditorWindow*				GetEditorWindowChild(int index);

	vector<EditorWindow*>		GetChildren() { return mChildren; }

	const string				GetID() { return mID; }

	virtual void				Initialize(float width, float height, ImTextureRef ref = nullptr);
	//GUIパネルのレイアウトを初期化
	virtual void				ResetLayoutFunction();

	virtual void				SetPanelColorTheme();

	virtual bool				MouseHoveredDisble();

	virtual void				ResetWindowPos(float width, float height);

	bool						WindowHoveredConfirmation();


	virtual void				BaseGUIPanelPopupMenu();

	virtual void				ClearPointer();

	bool						IsMouseHovered() const { return isMouseHovered; }

	float						GetWindowSizeWidth();
	float						GetWindowSizeHeight();
	//画面サイズを元にアスペクト比を計算
	ImVec2						GetAspectRatio();

	void						EnableResetLayout() { isResetLayout = true; }

	//ポップアップ用のカラー設定
	virtual void 				SetPopupColorTheme();
	virtual void 				ResetPopupColorTheme();

	bool						IsShow() { return mIsShow; }
	void						Enabled() { mIsShow = true; }
	void						Diabled() { mIsShow = false; }
};

