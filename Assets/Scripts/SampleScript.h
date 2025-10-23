#pragma once
#include "PrototypeEngine/ScriptComponent.h"
// #include "Actor.h" など、必要に応じてインクルード

class SampleScript : public ScriptComponent
{
public:
    // コンストラクタ
    SampleScript(class ActorObject* owner);

    // ライフサイクル関数のオーバーライド
    void Start() override;
    void Update(float deltaTime) override;
};
