#pragma once
#include "SDL3.h"
#include "Assimp.h"
#include "Collision.h"

#include "VertexArray.h"
/*
* ===エンジン内部処理/Engine internal processing===
*/

namespace MeshLayout
{
	//メッシュの要素数
	constexpr int MESH_VERTEXCOUNT = 8;
	//スキンメッシュの要素数
	constexpr int SKINMESH_VERTEXCOUNT = 13;
}

//マテリアル情報の構造体
struct MaterialInfo
{
	Vector4		sColor = Vector4();
	Vector3		sDiffuse = Vector3();
    Vector3     sAmbient = Vector3();
    Vector3     sSpecular = Vector3();
	float		sShininess = 0;

	float		sMetallic = 0;
	float		sRoughness = 0;
    Vector3     sEmissive  = Vector3();
};

//前方宣言
class Renderer;
class Texture;
class VertexArray;

//1つの3DMeshの管理クラス
//FBXファイルを読み込んでメッシュ情報を管理するクラス
class Mesh
{
private:
	// FBXファイルからJSONに変換処理
	bool						LoadFromFBX(const string& fileName, Renderer* renderer, int index);
	// AABBの当たり判定を配列で取得
	vector<AABB>				mBoxs;
	vector<OBB>					mOBBBoxs;
	// Meshのテクスチャを取得
	vector<Texture*>			mTextures;
	// MeshのVertexArrayの配列
	vector<VertexArray*>		mVertexArrays;
	// シェーダー名
	string						mShaderName;
	// 球の半径
	vector<float>				mRadiusArray;
	// マテリアル情報
	vector<MaterialInfo>		mMaterialInfo;

	vector<Vertex>				mVertices;
	vector<unsigned int>		mIndices;
public:
								Mesh();
								~Mesh();
	// Load
	// バイナリファイルからの読み込み処理
	bool						LoadFromMeshBin(const string& fileName, Renderer* renderer, int index = 0);

	bool						LoadFromSubMesh(const string& fbxPath, const string& localID);

	//Meshの数を取得
	int							CheckMeshIndex(const string& fileName, Renderer* renderer);
	// Unload mesh
	void						Unload();
	// Getter
	// MeshのVertexArrayを配列で取得
	vector<VertexArray*>		GetVertexArrays() 
	{
		return mVertexArrays; 
	}
	// テクスチャを配列で取得
	Texture*					GetTexture(size_t index);
	// シェーダー名を取得
	const string&				GetShaderName() const { return mShaderName; }
	// 球の半径を取得
	vector<float>				GetRadiusArray() const { return mRadiusArray; }
	// モデルのボックス型の座標を取得
	const vector<AABB>			GetBoxs() const { return mBoxs; }
	const vector<OBB>			GetOBBBoxs() const { return mOBBBoxs; }
	Sphere						GetAABBFromSphere();
	Capsule						GetAABBFromCapsule();
	//マテリアル情報取得
	vector<MaterialInfo>		GetMaterialInfo() { return mMaterialInfo; }
	void  SetMaterialInfo(const vector<MaterialInfo>& info) 
	{ 
		mMaterialInfo = info; 
	}

	const vector<Vertex>& GetVertices() const { return mVertices; }
	const vector<unsigned int>& GetIndices() const { return mIndices; }
};