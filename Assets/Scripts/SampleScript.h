#pragma once
#include "PrototypeEngine/ScriptComponent.h"

class SampleScript : public ScriptComponent
{
public:
    // Constructor
    SampleScript(class ActorObject* owner);

    // Overriding Lifecycle Functions
    void Start() override;

    void Update(float deltaTime) override;
};