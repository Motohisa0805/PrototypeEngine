#include "SceneViewEditor.h"

SceneViewEditor::SceneViewEditor()
	: sceneFBO(0)
	, sceneColorTex(0)
	, sceneDepthTex(0)
{
}

SceneViewEditor::~SceneViewEditor()
{
	if (sceneFBO != 0)
	{
		glDeleteFramebuffers(1, &sceneFBO);
		sceneFBO = 0;
	}
	if (sceneColorTex != 0)
	{
		glDeleteTextures(1, &sceneColorTex);
		sceneColorTex = 0;
	}
	if (sceneDepthTex != 0)
	{
		glDeleteTextures(1, &sceneDepthTex);
		sceneDepthTex = 0;
	}
}

void SceneViewEditor::CreateSceneFBO(int width, int height)
{
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    // カラーテクスチャ
    glGenTextures(1, &sceneColorTex);
    glBindTexture(GL_TEXTURE_2D, sceneColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, sceneColorTex, 0);

    // デプスバッファ
    glGenTextures(1, &sceneDepthTex);
    glBindTexture(GL_TEXTURE_2D, sceneDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height,
        0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, sceneDepthTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("SceneFBO not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
