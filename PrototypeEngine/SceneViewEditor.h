#pragma once
#include "Math.h"
#include <GL/glew.h>

// 3D�V�[���̕\�����Ǘ�����N���X
//ImGui��SceneViewPanel��GameViewPanel�Ŏg�p���Ă���
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