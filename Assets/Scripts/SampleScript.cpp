#include "SampleScript.h"
#include <iostream>

SampleScript::SampleScript(ActorObject* owner)
    : ScriptComponent(owner) // 基底クラスのコンストラクタを呼び出す
{
    mName = "SampleScript"; // Inspectorでの表示名を設定
}

void SampleScript::Start()
{
    // 初期化ロジックをここに記述
    std::cout << mName << "::Start() called!" << std::endl;
}

void SampleScript::Update(float deltaTime)
{
    // 毎フレームの更新ロジックをここに記述
}


// ----------------------------------------------------------------
// 自動登録の実行
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(SampleScript); // クラス名を渡すだけで自動登録される
