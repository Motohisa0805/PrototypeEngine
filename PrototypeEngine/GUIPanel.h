#pragma once
#include "Texture.h"
#include "Renderer.h"

//前方宣言
class Renderer;
//GUIのパネルの継承元クラス
//名前、マウスカーソルがパネル内にあるかどうかなどの処理を行っています。
class GUIPanel
{
protected:
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

	virtual bool		MouseHoveredDisble();

	virtual void		ResetWindowPos(float width, float height);

	bool				WindowHoveredConfirmation();

	virtual void		Draw(float width, float height, ImTextureRef ref = nullptr);

	virtual void        GUIPanelMenu();

	bool				IsMouseHovered() const { return isMouseHovered; }

	float				GetWindowSizeWidth();
	float				GetWindowSizeHeight();
	//画面サイズを元にアスペクト比を計算
	ImVec2				GetAspectRatio();

	void				EnableResetLayout() { isResetLayout = true; }
};

