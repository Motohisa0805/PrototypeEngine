#pragma once
#include "EditorTextureManager.h"
#include "Texture.h"
#include "Renderer.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

//前方宣言
class Renderer;
//GUIのパネルの継承元クラス
//名前、マウスカーソルがパネル内にあるかどうかなどの処理を行っています。
class GUIPanel
{
protected:
	struct RenameRequest
	{
		filesystem::path	oldPath;
		// 拡張子は oldPath.extension() を使って再結合する
		string				newStem;
	};

	Renderer*			mRenderer;
	bool				isMouseHovered;
	float				mWidthPos;
	float				mHeightPos;
	float				mWidthSize;
	float				mHeightSize;
	//GUIのレイアウトを初期位置に戻すフラグ
	bool				isResetLayout;
public:
	virtual const char* GetName() { return "BasePanel"; }
	GUIPanel(Renderer* renderer);

	virtual void		Initialize(float width, float height, ImTextureRef ref = nullptr);
	//GUIパネルのレイアウトを初期化
	virtual void		ResetLayoutFunction();

	virtual void        SetPanelColorTheme();

	virtual bool		MouseHoveredDisble();

	virtual void		ResetWindowPos(float width, float height);

	bool				WindowHoveredConfirmation();

	virtual void		Draw(float width, float height, ImTextureRef ref = nullptr);

	virtual void        BaseGUIPanelPopupMenu();

	virtual void		ClearPointer();

	bool				IsMouseHovered() const { return isMouseHovered; }

	float				GetWindowSizeWidth();
	float				GetWindowSizeHeight();
	//画面サイズを元にアスペクト比を計算
	ImVec2				GetAspectRatio();

	void				EnableResetLayout() { isResetLayout = true; }

	//ポップアップ用のカラー設定
	virtual void 				SetPopupColorTheme();
	virtual void 				ResetPopupColorTheme();
};

