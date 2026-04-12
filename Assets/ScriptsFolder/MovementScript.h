#pragma once
#include "PrototypeEngine/ScriptComponent.h"

class MovementScript : public ScriptComponent
{
public:
    // Constructor
    MovementScript(class ActorObject* owner);

    // Overriding Lifecycle Functions
    void Start() override;

    void Update(float deltaTime) override;
};