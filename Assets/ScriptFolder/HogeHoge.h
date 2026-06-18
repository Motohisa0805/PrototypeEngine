#pragma once
#include "PrototypeEngine/ScriptComponent.h"

class HogeHoge : public ScriptComponent
{
public:
    // Constructor
    HogeHoge(class Entity* owner);

    // Overriding Lifecycle Functions
    void Start() override;

    void Update(float deltaTime) override;
};