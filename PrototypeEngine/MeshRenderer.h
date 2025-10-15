#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Mesh;
//メッシュの描画を行うコンポーネントクラス
//読み込んだメッシュをセットして使用する
//GUI上の処理、描画、読み込み処理
class MeshRenderer : public Component
{
protected:
	//メッシュ配列
	vector<Mesh*>				mMeshs;
	//表示か非表示か
	bool						mVisible;
	//スケルタルフラグ
	bool						mIsSkeletal;
	//メッシュファイルパス
	string 						mFilePath;
	//メッシュのアルファ値
	float						mAlpha;
public:
								MeshRenderer(ActorObject* owner, bool isSkeletal = false);
								~MeshRenderer();
	// このメッシュコンポーネントを描画
	virtual void				Draw(Shader* shader);
	virtual void				DrawForShadowMap(Shader* shader);
	// メッシュコンポーネントで使用されるメッシュ/テクスチャインデックスを設定する
	virtual void				SetMesh(Mesh* mesh) 
	{
		mMeshs.push_back(mesh);
	}
	//既にセットされているメッシュにプラスで追加する形で設定
	virtual void				AddMeshs(const vector<Mesh*>& mesh)
	{
		mMeshs.insert(mMeshs.end(), mesh.begin(), mesh.end());
	}
	//読み込んだ複数のメッシュを設定
	virtual void				SetMeshs(const vector<Mesh*>& mesh)
	{
		mMeshs.clear();
		mMeshs = mesh;
	}

	vector<Mesh*>				GetMeshs() const { return mMeshs; }

	void						SetVisible(bool visible) { mVisible = visible; }
	bool						GetVisible() const { return mVisible; }

	bool						GetIsSkeletal() const { return mIsSkeletal; }
	// ファイルパスを設定するSetterを追加
	void						SetMeshFilePath(const std::string& path) { mFilePath = path; }
	const std::string&			GetMeshFilePath() const { return mFilePath; }

	void						SetMaterialAlpha(float alpha);

	void						Serialize(json& j) const override;
	void						Deserialize(const json& j)override;

	void						DrawGUI()override;

};