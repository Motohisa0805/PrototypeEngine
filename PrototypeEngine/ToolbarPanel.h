#pragma once
#include "GUIPanel.h"

//ゲームの再生、停止、一時停止、コマ送りなど画面の上部分のGUIの描画処理クラス
class ToolbarPanel : public GUIPanel
{
private:

	Texture* mPlayButtonTexture;
	Texture* mPauseButtonTexture;
	Texture* mStopButtonTexture;
	Texture* mFrameByFrameButtonTexture;
public:
	ToolbarPanel(class Renderer* renderer);
	~ToolbarPanel();
	void Initialize(float width, float height, ImTextureRef ref = nullptr)override;
	void ResetWindowPos(float width, float height)override;
	void Draw(float width, float height,ImTextureRef ref = nullptr)override;
	const char* GetName()override { return "Toolbar"; }
};

