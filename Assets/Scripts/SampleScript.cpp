#include "pch.h"
#include "SampleScript.h"
#include <iostream>

SampleScript::SampleScript(ActorObject* owner)
    : ScriptComponent(owner)
{
    //Set the display name in the Inspector
    mName = "SampleScript"; 
}

//Write the initialization logic here
void SampleScript::Start()
{
    std::cout << mName << "::Start() called!" << std::endl;
}

//Write the update logic for each frame here
void SampleScript::Update(float deltaTime)
{

}

// ----------------------------------------------------------------
// Execute automatic registration
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(SampleScript);