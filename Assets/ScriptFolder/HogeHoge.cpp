#include "pch.h"
#include "HogeHoge.h"
#include <iostream>

HogeHoge::HogeHoge(Entity* owner)
    : ScriptComponent(owner)
{
    //Set the display name in the Inspector
    mName = "HogeHoge"; 
}

//Write the initialization logic here
void HogeHoge::Start()
{
    std::cout << mName << "::Start() called!" << std::endl;
}

//Write the update logic for each frame here
void HogeHoge::Update(float deltaTime)
{

}

// ----------------------------------------------------------------
// Execute automatic registration
// ----------------------------------------------------------------
REGISTER_SCRIPT_COMPONENT(HogeHoge);