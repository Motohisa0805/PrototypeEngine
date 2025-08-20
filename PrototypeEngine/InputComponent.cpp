#include "InputComponent.h"
#include "Actor.h"

InputComponent::InputComponent(class ActorObject* owner)
	:MoveComponent(owner)
	, mForwardKey(0)
	, mBackKey(0)
	, mClockwiseKey(0)
	, mCounterClockwiseKey(0)
{

}

void InputComponent::ProcessInput(const struct InputState& keyState)
{
	/*
	// Calculate forward speed for MoveComponent
	float forwardSpeed = 0.0f;
	if (keyState.Keyboard.GetKeyState(mForwardKey))
	{
		forwardSpeed += mMaxForwardSpeed;
	}
	if (keyState.Keyboard.GetKeyState(mBackKey))
	{
		forwardSpeed -= mMaxForwardSpeed;
	}
	SetForwardSpeed(forwardSpeed);

	// Calculate angular speed for MoveComponent
	float angularSpeed = 0.0f;
	if (keyState.Keyboard.GetKeyState(mClockwiseKey))
	{
		angularSpeed += mMaxAngularSpeed;
	}
	if (keyState.Keyboard.GetKeyState(mCounterClockwiseKey))
	{
		angularSpeed -= mMaxAngularSpeed;
	}
	SetAngularSpeed(angularSpeed);
	*/
}