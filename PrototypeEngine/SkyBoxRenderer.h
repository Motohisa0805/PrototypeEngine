#pragma once
#include "Math.h"
#include "Texture.h"
#include "Shader.h"
#include "GameWinMain.h"
#include "BaseScene.h"

#define SKYBOXVERTEX_COUNT 36

class Texture;
class VertexArray;

//�X�J�C�{�b�N�X��`�悷��N���X
// Cubemap���g���ė����̂̃��b�V����`�悷��
// �����̂̃��b�V���͗��ʕ`��p
// �����̂̃��b�V���́AOpenGL�̃f�t�H���g�̗����̃��b�V�����g�p
class SkyBoxRenderer
{
private:
    // Cubemap�p
    Texture*                                    mTexture;
    std::unordered_map<string,Texture*>	        mTextures;
    // ���ʕ`��p�̗����̃��b�V��
    VertexArray*                                mCubeVAO;  
	// ���_��
    int                                         mVerticesCount;
public:
    SkyBoxRenderer();
    ~SkyBoxRenderer();

    // Cubemap�ǂݍ���
    void        Load(const std::string& file, int faceSize = 512);

    void        Update(float deltaTime) {}

    void        Draw(class Shader* shader, const Matrix4& view, const Matrix4& proj);

    void        UnLoad();

    Texture*    GetTexture() const { return mTexture; }
};

