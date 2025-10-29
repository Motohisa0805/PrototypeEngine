#pragma once
#include "PrototypeEngine/ScriptComponent.h"

class NeScript : public ScriptComponent
{
public:
    // Constructor
    NeScript(class ActorObject* owner);

    // Overriding Lifecycle Functions
    void Start() override;

    void Update(float deltaTime) override;
};