#include "pch.h"
#include "MovementScript.h"
#include <iostream>

MovementScript::MovementScript(ActorObject* owner)
    : ScriptComponent(owner)
{
    //Set the display name in the Inspector
    mName = "MovementScript"; 
}

//Write the initialization logic here
void MovementScript::Start()
{
    std::cout << mName << "::Start() called!" << std::endl;
}

//Write the update logic for each frame here
void MovementScript::Update(float deltaTime)
{

}

// ----------------------------------------------------------------
// Execute automatic registration
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(MovementScript);