#pragma once
#include <GL/glew.h>
#include "StandardLibrary.h"
#include "Math.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

// 頂点レイアウトの定義
namespace VertexLayout
{
	//各型のバイト数
	constexpr size_t FLOAT_SIZE = sizeof(float);
	constexpr size_t UBYTE_SIZE = sizeof(unsigned char);

	constexpr GLsizei NUM_VERTEX_ARRAYS = 1;

	constexpr GLsizei NUM_VERTEX_BUFFERS = 1;

	constexpr GLsizei NUM_INDEX_BUFFERS = 1;

	//***glEnableVertexAttribArrayのアクセスインデックス***
	//位置の頂点属性のインデックス
	constexpr size_t  POSITION_INDEX = 0;
	//法線の頂点属性のインデックス
	constexpr size_t  NORMAL_INDEX = 1;
	//テクスチャ座標(UV)の頂点属性のインデックス
	constexpr size_t  MESH_TEXUV_INDEX = 2;
	//ウェイトの頂点属性のインデックス
	constexpr size_t  BONEWEIGHT_INDEX = 3;
	//ボーンインデックス・ボーンのウェイト・テクスチャ情報の頂点属性のインデックス
	constexpr size_t  SKINMESH_TEXUV_INDEX = 4;


	// 属性の構成（floatサイズ前提）
	constexpr size_t POSITION_COMPONENTS = 3;
	constexpr size_t NORMAL_COMPONENTS = 3;
	constexpr size_t TEXCOORD_COMPONENTS = 2;
	constexpr size_t WEIGHT_COMPONENTS = 4;
	constexpr size_t BONE_INDEX_COUNT = 4;

	//Positionオフセット値
	constexpr size_t POSITION_OFFSET = 0;
	//Normalオフセット値
	constexpr size_t  NORMAL_OFFSET = POSITION_COMPONENTS;
	//Textrue座標のオフセット値
	constexpr size_t  TEXTURE_OFFSET = POSITION_COMPONENTS + NORMAL_COMPONENTS;
	//スキニングオフセット値
	constexpr size_t  SKINNING_OFFSET = POSITION_COMPONENTS + NORMAL_COMPONENTS;
	//スキンウェイトオフセット値
	constexpr size_t  SKINWEIGHT_OFFSET = POSITION_COMPONENTS + NORMAL_COMPONENTS;
	//テクスチャオフセット値
	constexpr size_t  SKINTEXTURE_OFFSET = POSITION_COMPONENTS + NORMAL_COMPONENTS + WEIGHT_COMPONENTS;


	//Meshオブジェクトの頂点サイズ
	constexpr size_t  MESHOBJECT_SIZE = POSITION_COMPONENTS + NORMAL_COMPONENTS + TEXCOORD_COMPONENTS;
	//SkinMeshオブジェクトの頂点サイズ
	constexpr size_t  SKINMESHOBJECT_SIZE = POSITION_COMPONENTS + NORMAL_COMPONENTS + TEXCOORD_COMPONENTS + BONE_INDEX_COUNT;

	//各頂点のサイズ(NormTex)
	constexpr size_t  NORMTEX_SIZE = MESHOBJECT_SIZE * FLOAT_SIZE;
	//各頂点のサイズ(NormSkinTex)
	constexpr size_t  NORMSKINTEX_SIZE = SKINMESHOBJECT_SIZE * FLOAT_SIZE + WEIGHT_COMPONENTS * UBYTE_SIZE;
}

struct AxisVertex
{
	Vector3 position;
	Vector3 color;
};

//書籍元を改造したファイル
//頂点配列オブジェクト(Vertex Array Object)を管理するクラス
class VertexArray
{
private:
	//vertex bufferの頂点数
	unsigned int	mNumVerts;
	// index bufferのインデックス数
	unsigned int	mNumIndices;
	// 頂点バッファのOpenGL ID
	unsigned int	mVertexBuffer;
	// インデックスバッファのOpenGL ID
	unsigned int	mIndexBuffer;
	// 頂点配列オブジェクトのOpenGL ID
	unsigned int	mVertexArray;
public:
	// 頂点レイアウトタグ
	enum Layout
	{
		PosNormTex,
		PosNormSkinTex,
		Pos,
	};

					VertexArray(const void* verts, unsigned int numVerts, Layout layout,
								const unsigned int* indices, unsigned int numIndices);
					VertexArray(float fillAmount, int maxSegments);

					VertexArray(const float* verts, unsigned int numVerts);
					VertexArray(const std::vector<AxisVertex>& verts);
					~VertexArray();

	void			SetActive();
	unsigned int	GetNumIndices() const { return mNumIndices; }
	unsigned int	GetNumVerts() const { return mNumVerts; }
};