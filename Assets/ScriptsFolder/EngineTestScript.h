#pragma once
#include "PrototypeEngine/ScriptComponent.h"

class EngineTestScript : public ScriptComponent
{
public:
    // Constructor
    EngineTestScript(class ActorObject* owner);

    // Overriding Lifecycle Functions
    void Start() override;

    void Update(float deltaTime) override;
};