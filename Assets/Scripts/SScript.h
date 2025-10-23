#pragma once
#include "PrototypeEngine/ScriptComponent.h"
// #include "Actor.h" など、必要に応じてインクルード

class SScript : public ScriptComponent
{
private:
	//メッシュファイルパス
	string 						mFilePath;
	//メッシュのアルファ値
	int							mAlpha;

	//リフレクションマクロの適用
	REFLECT_CLASS_BEGIN(SScript, Component)
	REFLECT_PROPERTY(INT, mAlpha, GetAlpha, SetMaterialAlpha);
	REFLECT_END
public:
    // コンストラクタ
    SScript(class ActorObject* owner);

    // ライフサイクル関数のオーバーライド
    void Start() override;
    void Update(float deltaTime) override;
};
