#include "DebugGrid.h"
#include "Shader.h"
#include "VertexArray.h"

DebugGrid::DebugGrid(float gridSize, int gridCount) : mGridVAO(nullptr)
{
    std::vector<float> verts;
    int                half = gridCount / 2;
    for (int i = -half; i <= half; ++i)
    {
        // Z軸方向の線（X固定）
        verts.push_back(i * gridSize);
        verts.push_back(0);
        verts.push_back(-half * gridSize);
        verts.push_back(i * gridSize);
        verts.push_back(0);
        verts.push_back(half * gridSize);
        // X軸方向の線（Z固定）
        verts.push_back(-half * gridSize);
        verts.push_back(0);
        verts.push_back(i * gridSize);
        verts.push_back(half * gridSize);
        verts.push_back(0);
        verts.push_back(i * gridSize);
    }

    mGridVAO = new VertexArray(verts.data(),
                               static_cast<unsigned int>(verts.size() / 3));
}

DebugGrid::~DebugGrid()
{
    if (mGridVAO)
    {
        delete mGridVAO;
        mGridVAO = nullptr;
    }
}

void DebugGrid::GenerateGrid(float gridSize, int gridCount,
                             const Vector3& center)
{
    // 既存のVAOを削除
    if (mGridVAO)
    {
        delete mGridVAO;
        mGridVAO = nullptr;
    }

    vector<float> verts;
    int           half  = gridCount / 2;
    float         start = -half * gridSize;
    float         end   = half * gridSize;

    // グリッド線の生成ロジック
    for (int i = -half; i <= half; ++i)
    {
        float offset = i * gridSize;

        // X軸に平行な線 (Z固定)
        verts.push_back(center.x + offset);
        verts.push_back(center.y);
        verts.push_back(center.z + start);
        verts.push_back(center.x + offset);
        verts.push_back(center.y);
        verts.push_back(center.z + end);

        // Z軸に平行な線 (X固定)
        verts.push_back(center.x + start);
        verts.push_back(center.y);
        verts.push_back(center.z + offset);
        verts.push_back(center.x + end);
        verts.push_back(center.y);
        verts.push_back(center.z + offset);
    }

    mGridVAO = new VertexArray(verts.data(),
                               static_cast<unsigned int>(verts.size() / 3));
    mCurrentGridSize  = gridSize;
    mCurrentGridCount = gridCount;
    mGridCenter       = center;
}

void DebugGrid::Draw(Shader* shader, const Matrix4& viewProj,
                     const Vector3& cameraPos)
{
    // 1.グリッドの中心を計算(最も近い整数座標にスナップ)
    Vector3 newCenter;
    newCenter.x = std::round(cameraPos.x);
    newCenter.y = 0;
    newCenter.z = std::round(cameraPos.z);

    // 2.ズームレベルに応じたグリッドサイズ/密度を決定
    //  ビュー行列の要素から距離を測るか、カメラ位置を直接使うなどして、
    //  カメラとグリッド平面 (Y=0) の距離を推定します。
    float distToPlane = Math::Abs(cameraPos.y);

    // 基準サイズ
    float newGridSize = 1.0f;
    // 適切な描画範囲
    int newGridCount = 100;

    // ズームレベルに応じてサイズを調整する
    if (distToPlane > 50.0f)
    {
        newGridSize = 10.0f;
    }
    else if (distToPlane > 100.0f)
    {
        newGridSize = 50.0f;
    }

    // 3.グリッドの中心またはサイズが変わった場合のみ再生成
    if (mGridCenter.x != newCenter.x || mGridCenter.z != newCenter.z ||
        mCurrentGridSize != newGridSize || mCurrentGridCount != newGridCount)
    {
        GenerateGrid(newGridSize, newGridCount, newCenter);
    }

    // 4.描画
    if (mGridVAO)
    {
        shader->SetActive();
        shader->SetMatrixUniform("uViewProj", viewProj);
        shader->SetVectorUniform("uColor", Vector3(1.0f, 1.0f, 1.0f));

        mGridVAO->SetActive();
        glLineWidth(1.0f); // 線の太さを3ピクセルに設定
        glDrawArrays(GL_LINES, 0, mGridVAO->GetNumVerts());
    }
}
