#include "NeScript.h"
#include <iostream>

NeScript::NeScript(ActorObject* owner)
    : ScriptComponent(owner)
{
    //Set the display name in the Inspector
    mName = "NeScript"; 
}

//Write the initialization logic here
void NeScript::Start()
{
    std::cout << mName << "::Start() called!" << std::endl;
}

//Write the update logic for each frame here
void NeScript::Update(float deltaTime)
{

}

// ----------------------------------------------------------------
// Execute automatic registration
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(NeScript);