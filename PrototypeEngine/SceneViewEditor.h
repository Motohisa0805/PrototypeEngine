#pragma once
#include "Math.h"
#include <GL/glew.h>

class SceneViewEditor
{
private:
	GLuint sceneFBO;
	GLuint sceneColorTex;
	GLuint sceneDepthTex;
public:
	SceneViewEditor();
	~SceneViewEditor();

	void CreateSceneFBO(int width, int height);

	GLuint GetSceneFBO() const { return sceneFBO; }
	GLuint GetSceneColorTex() const { return sceneColorTex; }
	GLuint GetSceneDepthTex() const { return sceneDepthTex; }
};

