#include "SceneViewEditor.h"

SceneViewEditor::SceneViewEditor()
	: mSceneFBO(0)
    , mSceneColorTex(0)
    , mSceneDepthTex(0)
{
}

SceneViewEditor::~SceneViewEditor()
{

    if (mSceneFBO != 0)
    {
        glDeleteFramebuffers(1, &mSceneFBO);
        mSceneFBO = 0;
    }
    if (mSceneColorTex != 0)
    {
        glDeleteTextures(1, &mSceneColorTex);
        mSceneColorTex = 0;
    }
    if (mSceneDepthTex != 0)
    {
        glDeleteTextures(1, &mSceneDepthTex);
        mSceneDepthTex = 0;
    }
}

void SceneViewEditor::CreateSceneFBO(int width, int height)
{
    glGenFramebuffers(1, &mSceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mSceneFBO);

    // カラーテクスチャ
    glGenTextures(1, &mSceneColorTex);
    glBindTexture(GL_TEXTURE_2D, mSceneColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, mSceneColorTex, 0);

    // デプスバッファ
    glGenTextures(1, &mSceneDepthTex);
    glBindTexture(GL_TEXTURE_2D, mSceneDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height,
        0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, mSceneDepthTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("SceneFBO not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mCurrentSceneSize = Vector2((float)width, (float)height);
}

bool SceneViewEditor::NeedsResize(Vector2 size)
{
	bool b = (size.x != mCurrentSceneSize.x || size.y != mCurrentSceneSize.y);
    return b;
}
