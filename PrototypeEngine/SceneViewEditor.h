#pragma once
#include "Math.h"
#include <GL/glew.h>

// 3Dシーンの表示を管理するクラス
//ImGuiのSceneViewPanelとGameViewPanelで使用している
class SceneViewEditor
{
private:
	GLuint mSceneFBO;
	GLuint mSceneColorTex;
	GLuint mSceneDepthTex;

	Vector2 mCurrentGameSceneSize;
	Vector2 mCurrentSceneSize;
public:
	SceneViewEditor();
	~SceneViewEditor();
	void	CreateSceneFBO(int width, int height);

	bool	NeedsResize(Vector2 size);

	GLuint	GetSceneFBO() const { return mSceneFBO; }
	GLuint	GetSceneColorTex() const { return mSceneColorTex; }
	GLuint	GetSceneDepthTex() const { return mSceneDepthTex; }
};