#include "SScript.h"
#include <iostream>

SScript::SScript(ActorObject* owner)
    : ScriptComponent(owner) // 基底クラスのコンストラクタを呼び出す
{
    mName = "SScript"; // Inspectorでの表示名を設定
}

void SScript::Start()
{
    // 初期化ロジックをここに記述
    std::cout << mName << "::Start() called!" << std::endl;
}

void SScript::Update(float deltaTime)
{
    // 毎フレームの更新ロジックをここに記述
}


// ----------------------------------------------------------------
// 自動登録の実行
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(SScript); // クラス名を渡すだけで自動登録される
