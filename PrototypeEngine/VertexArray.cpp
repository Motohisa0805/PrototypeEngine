#include "VertexArray.h"

VertexArray::VertexArray(const void* verts, unsigned int numVerts,
                         Layout layout, const unsigned int* indices,
                         unsigned int numIndices)
    : mNumVerts(numVerts), mNumIndices(numIndices)
{
    // 頂点配列を作成する
    glGenVertexArrays(VertexLayout::NUM_VERTEX_ARRAYS, &mVertexArray);
    glBindVertexArray(mVertexArray);

    GLsizei stride = (layout == PosNormSkinTex) ? sizeof(Vertex) : sizeof(StaticVertex);

    // 頂点バッファを作成する
    glGenBuffers(VertexLayout::NUM_VERTEX_BUFFERS, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVerts * stride, verts, GL_STATIC_DRAW);

    // インデックスバッファを作成する
    glGenBuffers(VertexLayout::NUM_INDEX_BUFFERS, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int),
                 indices, GL_STATIC_DRAW);

    // 頂点属性を指定してください
    if (layout == PosNormTex)
    {
        // Positionは3つの浮動小数点
        glEnableVertexAttribArray(VertexLayout::ATTRIB_POSITION);
        glVertexAttribPointer(VertexLayout::ATTRIB_POSITION,3, GL_FLOAT,
                              GL_FALSE, stride, (void*)offsetof(StaticVertex,sPos));

        // Normalは3つの浮動小数点
        glEnableVertexAttribArray(VertexLayout::ATTRIB_NORMAL);
        glVertexAttribPointer(VertexLayout::ATTRIB_NORMAL,3, GL_FLOAT,
                              GL_FALSE, stride, (void*)offsetof(StaticVertex, sNormal));

        // テクスチャ座標は2つの浮動小数点数
        glEnableVertexAttribArray(VertexLayout::ATTRIB_TEXCOORD);
        glVertexAttribPointer(VertexLayout::ATTRIB_TEXCOORD, 2,
                              GL_FLOAT, GL_FALSE, stride,(void*)offsetof(StaticVertex, sUV));
    }
    else if (layout == PosNormSkinTex)
    {
        // スキニングメッシュ用の追加属性
        // Positionは3つの浮動小数点
        glEnableVertexAttribArray(VertexLayout::ATTRIB_POSITION);
        glVertexAttribPointer(VertexLayout::ATTRIB_POSITION, 3, GL_FLOAT,
                              GL_FALSE, stride, (void*)offsetof(Vertex, pos));

        // Normalは3つの浮動小数点
        glEnableVertexAttribArray(VertexLayout::ATTRIB_NORMAL);
        glVertexAttribPointer(VertexLayout::ATTRIB_NORMAL, 3, GL_FLOAT,
                              GL_FALSE, stride,
                              (void*)offsetof(Vertex, normal));

        // テクスチャ座標は2つの浮動小数点数
        glEnableVertexAttribArray(VertexLayout::ATTRIB_TEXCOORD);
        glVertexAttribPointer(VertexLayout::ATTRIB_TEXCOORD, 2, GL_FLOAT,
                              GL_FALSE, stride, (void*)offsetof(Vertex, uv));

        // Bone IDs（整数として保持）
        glEnableVertexAttribArray(VertexLayout::ATTRIB_BONE_IDS);
        glVertexAttribIPointer(VertexLayout::ATTRIB_BONE_IDS, 4,
                               GL_UNSIGNED_BYTE, stride,
                               (void*)offsetof(Vertex, boneIDs));

        // Bone Weights（浮動小数点に変換）
        glEnableVertexAttribArray(VertexLayout::ATTRIB_BONE_WEIGHTS);
        glVertexAttribPointer(VertexLayout::ATTRIB_BONE_WEIGHTS, 4, GL_FLOAT,
                              GL_FALSE, stride,
                              (void*)offsetof(Vertex, weights));
    }
    else if (layout == Pos)
    {
        glEnableVertexAttribArray(VertexLayout::ATTRIB_POSITION);
        glVertexAttribPointer(VertexLayout::ATTRIB_POSITION, 3, GL_FLOAT,
                              GL_FALSE, sizeof(float) * 3, 0);
    }
}

VertexArray::VertexArray(float fillAmount, int maxSegments)
    : mNumVerts(0)
    , mNumIndices(0)
    , mVertexArray(0)
    , mVertexBuffer(0)
    , mIndexBuffer(0)
{
    struct Vertex
    {
        float position[VertexLayout::POSITION_COMPONENTS];
        float texCoord[VertexLayout::TEXCOORD_COMPONENTS];
    };

    int   segments  = std::max(1, static_cast<int>(maxSegments * fillAmount));
    float angleStep = fillAmount * 2.0f * Math::Pi / segments;

    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    // 中心点（原点、UV 中心）
    vertices.push_back({{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f}});

    // 外周の扇形頂点を生成
    for (int i = 0; i <= segments; ++i)
    {
        float angle = i * angleStep;
        float x     = cosf(angle) * 0.5f;
        float y     = sinf(angle) * 0.5f;

        vertices.push_back({
            {x, y, 0.0f}, {x + 0.5f, y + 0.5f} // UV中心(0.5, 0.5)
        });
    }

    // インデックス（GL_TRIANGLES の三角形ファン）
    for (int i = 1; i <= segments; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    mNumVerts = static_cast<unsigned int>(indices.size());

    // OpenGLバッファ生成と設定
    glGenVertexArrays(VertexLayout::NUM_VERTEX_ARRAYS, &mVertexArray);
    glGenBuffers(VertexLayout::NUM_VERTEX_BUFFERS, &mVertexBuffer);
    glGenBuffers(VertexLayout::NUM_INDEX_BUFFERS, &mIndexBuffer);

    glBindVertexArray(mVertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // 頂点属性（位置：location=0、UV：location=2）
    glEnableVertexAttribArray(VertexLayout::ATTRIB_POSITION);
    glVertexAttribPointer(VertexLayout::ATTRIB_POSITION,
                          VertexLayout::POSITION_COMPONENTS, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(VertexLayout::ATTRIB_TEXCOORD);
    glVertexAttribPointer(VertexLayout::ATTRIB_TEXCOORD,
                          VertexLayout::TEXCOORD_COMPONENTS, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
}

VertexArray::VertexArray(const float* verts, unsigned int numVerts)
    : mNumVerts(numVerts)
    , mNumIndices(0)
    , mVertexArray(0)
    , mVertexBuffer(0)
    , mIndexBuffer(0)
{
    glGenVertexArrays(VertexLayout::NUM_VERTEX_ARRAYS, &mVertexArray);
    glGenBuffers(VertexLayout::NUM_VERTEX_BUFFERS, &mVertexBuffer);

    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, verts,
                 GL_STATIC_DRAW);

    // 位置属性だけ（vec3）
    glEnableVertexAttribArray(VertexLayout::ATTRIB_POSITION);
    glVertexAttribPointer(VertexLayout::ATTRIB_POSITION,
                          VertexLayout::POSITION_COMPONENTS, GL_FLOAT, GL_FALSE,
                          VertexLayout::POSITION_COMPONENTS * sizeof(float),
                          (void*)0);

    glBindVertexArray(0);
}

VertexArray::VertexArray(const std::vector<AxisVertex>& verts)
    : mNumVerts(static_cast<unsigned int>(verts.size()))
    , mNumIndices(0)
    , mVertexArray(0)
    , mVertexBuffer(0)
    , mIndexBuffer(0)
{
    glGenVertexArrays(1, &mVertexArray);
    glGenBuffers(1, &mVertexBuffer);

    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(AxisVertex),
                 verts.data(), GL_STATIC_DRAW);

    // 位置属性（location = 0）
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex),
                          (void*)offsetof(AxisVertex, position));

    // 色属性（location = 1）
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex),
                          (void*)offsetof(AxisVertex, color));

    glBindVertexArray(0);
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(VertexLayout::NUM_VERTEX_BUFFERS, &mVertexBuffer);
    glDeleteBuffers(VertexLayout::NUM_INDEX_BUFFERS, &mIndexBuffer);
    glDeleteVertexArrays(VertexLayout::NUM_VERTEX_ARRAYS, &mVertexArray);
}

void VertexArray::SetActive() { glBindVertexArray(mVertexArray); }
