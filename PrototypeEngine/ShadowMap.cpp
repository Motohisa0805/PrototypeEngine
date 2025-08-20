#include "ShadowMap.h"

ShadowMap::ShadowMap()
    : mFBO(0)
    , mDepthTexture(0)
    , mWidth(0)
    , mHeight(0)
{
}

ShadowMap::~ShadowMap()
{
    if (mDepthTexture != 0)
        glDeleteTextures(1, &mDepthTexture);
    if (mFBO != 0)
        glDeleteFramebuffers(1, &mFBO);
}

bool ShadowMap::Initialize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    // 深度テクスチャの生成
    glGenTextures(1, &mDepthTexture);
    glBindTexture(GL_TEXTURE_2D, mDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // フィルタリング設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // ラップモード（境界は影なしとして白）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 比較モードと比較関数を有効にする（これが sampler2DShadow に必要）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); // もしくは GL_LESS

    // フレームバッファの生成と設定
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);

    // 深度のみを使用（カラー出力なし）
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // 正常にフレームバッファが設定されたか確認
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Shadow map framebuffer is not complete!" << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void ShadowMap::BeginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glViewport(0, 0, mWidth, mHeight);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // カリング設定
    glEnable(GL_CULL_FACE);
    // シャドウアクネ防止のため前面をカリング
    glCullFace(GL_FRONT);   
}

void ShadowMap::EndRender()
{
    // カリング状態を元に戻す
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::UpdateLightMatrix(const Vector3& lightDir, const Vector3& target)
{
    Vector3 lightPos = target - lightDir * 20.0f; // ライト位置
    Matrix4 view = Matrix4::CreateLookAt(lightPos, target, Vector3::UnitY);

    // 平行光源向け正射影
    Matrix4 proj = Matrix4::CreateOrtho(50.0f, 50.0f, 1.0f, 50.0f);
    mLightViewProj = view * proj;
}
