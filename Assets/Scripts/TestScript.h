#pragma once
#include "PrototypeEngine/ScriptComponent.h"

class TestScript : public ScriptComponent
{
public:
    // Constructor
    TestScript(class ActorObject* owner);

    // Overriding Lifecycle Functions
    void Start() override;

    void Update(float deltaTime) override;
};