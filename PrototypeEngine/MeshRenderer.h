#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//前方宣言
class Mesh;
//メッシュの描画を行うクラス
//読み込んだメッシュをセットして使用する
class MeshRenderer : public Component
{
protected:
	vector<Mesh*>				mMeshs;
	size_t						mTextureIndex;
	bool						mVisible;
	bool						mIsSkeletal;
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

	virtual void				SetMeshs(const vector<Mesh*>& mesh)
	{
		mMeshs.insert(mMeshs.end(), mesh.begin(), mesh.end());
	}

	void						SetTextureIndex(size_t index) { mTextureIndex = index; }

	void						SetVisible(bool visible) { mVisible = visible; }
	bool						GetVisible() const { return mVisible; }

	bool						GetIsSkeletal() const { return mIsSkeletal; }

	vector<Mesh*>				GetMeshs() const { return mMeshs; }

    void SetMaterialAlpha(float alpha)
	{
		float a = Math::Clamp(alpha, 0.0f, 1.0f);
		if (!mMeshs.empty())
		{
			for (auto& mesh : mMeshs)
			{
				vector<MaterialInfo> info = mesh->GetMaterialInfo();
				for(int i = 0; i < info.size(); ++i)
				{
					info[i].Color = Vector4(info[i].Color.x, info[i].Color.y, info[i].Color.z,a);
				}
				mesh->SetMaterialInfo(info);
			}
		}
	}
};