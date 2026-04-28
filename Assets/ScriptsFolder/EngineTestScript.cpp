#include "pch.h"
#include "EngineTestScript.h"
#include <iostream>

EngineTestScript::EngineTestScript(ActorObject* owner)
    : ScriptComponent(owner)
{
    //Set the display name in the Inspector
    mName = "EngineTestScript"; 
}

//Write the initialization logic here
void EngineTestScript::Start()
{
    std::cout << mName << "::Start() called!" << std::endl;
}

//Write the update logic for each frame here
void EngineTestScript::Update(float deltaTime)
{

}

// ----------------------------------------------------------------
// Execute automatic registration
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(EngineTestScript);