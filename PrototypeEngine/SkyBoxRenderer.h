#pragma once
#include "Math.h"
#include "Texture.h"
#include "Shader.h"
#include "GameWinMain.h"
#include "BaseScene.h"

#define SKYBOXVERTEX_COUNT 36

//スカイボックスを描画するクラス
// Cubemapを使って立方体のメッシュを描画する
// 立方体のメッシュは裏面描画用
// 立方体のメッシュは、OpenGLのデフォルトの立方体メッシュを使用
class SkyBoxRenderer
{
private:
    // Cubemap用
    class Texture*                              mTexture;
    std::unordered_map<string, class Texture*>	mTextures;
    // 裏面描画用の立方体メッシュ
    class VertexArray*                          mCubeVAO;  
	// 頂点数
    int                                         mVerticesCount;
public:
    SkyBoxRenderer();
    ~SkyBoxRenderer();

    // Cubemap読み込み
    void Load(const std::string& file, int faceSize = 512);

    void Update(float deltaTime) {}

    void Draw(class Shader* shader, const Matrix4& view, const Matrix4& proj);

    void UnLoad();

    class Texture* GetTexture() const { return mTexture; }
};

