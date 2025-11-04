#include "pch.h"
#include "TestScript.h"
#include <iostream>

TestScript::TestScript(ActorObject* owner)
    : ScriptComponent(owner)
{
    //Set the display name in the Inspector
    mName = "TestScript"; 
}

//Write the initialization logic here
void TestScript::Start()
{
    std::cout << mName << "::Start() called!" << std::endl;
}

//Write the update logic for each frame here
void TestScript::Update(float deltaTime)
{

}

// ----------------------------------------------------------------
// Execute automatic registration
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(TestScript);