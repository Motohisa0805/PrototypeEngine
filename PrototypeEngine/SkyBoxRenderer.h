#pragma once
#include "Math.h"
#include "Typedefs.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"
#include "GameWinMain.h"

#define SKYBOXVERTEX_COUNT 36

//TODO : シーンごとにスカイボックスをキープ・セーブ機能がないため後程追加予定

//スカイボックスを描画するクラス
// Cubemapを使って立方体のメッシュを描画する
// 立方体のメッシュは裏面描画用
// 立方体のメッシュは、OpenGLのデフォルトの立方体メッシュを使用
class SkyBoxRenderer
{
private:
    // Cubemap用
    Texture*                                    mLoadTexture;
    std::unordered_map<string,Texture*>	        mTexturesMap;

    string                                      mLoadFilePath;

    // 裏面描画用の立方体メッシュ
    VertexArray*                                mCubeVAO;  
public:
    SkyBoxRenderer();
    ~SkyBoxRenderer();

    // Cubemap読み込み
    void        Load(const std::string& file, int faceSize = 512);

    void        Update(float deltaTime) {}

    void        Draw(class Shader* shader, const Matrix4& view, const Matrix4& proj);

    void        UnLoad();

    Texture*    GetTexture() const { return mLoadTexture; }

    string      GetLoadFilePath() { return mLoadFilePath; }
};

