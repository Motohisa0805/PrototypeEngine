#include "DebugGrid.h"

DebugGrid::DebugGrid(float gridSize, int gridCount)
	: mGridVAO(nullptr)
{
    std::vector<float> verts;
    int half = gridCount / 2;
    for (int i = -half; i <= half; ++i) {
        // Z�������̐��iX�Œ�j
        verts.push_back(i * gridSize); verts.push_back(0); verts.push_back(-half * gridSize);
        verts.push_back(i * gridSize); verts.push_back(0); verts.push_back(half * gridSize);
        // X�������̐��iZ�Œ�j
        verts.push_back(-half * gridSize); verts.push_back(0); verts.push_back(i * gridSize);
        verts.push_back(half * gridSize);  verts.push_back(0); verts.push_back(i * gridSize);
    }

    mGridVAO = new VertexArray(verts.data(), static_cast<unsigned int>(verts.size() / 3));
}

DebugGrid::~DebugGrid()
{
    if (mGridVAO)
    {
        delete mGridVAO;
        mGridVAO = nullptr;
    }
}

void DebugGrid::Draw(Shader* shader, const Matrix4& viewProj)
{
    shader->SetActive();
    shader->SetMatrixUniform("uViewProj", viewProj);
    shader->SetVectorUniform("uColor", Vector3(1.0f, 1.0f, 1.0f));

    mGridVAO->SetActive();
    glLineWidth(1.0f); // ���̑�����3�s�N�Z���ɐݒ�
    glDrawArrays(GL_LINES, 0, mGridVAO->GetNumVerts());
}
