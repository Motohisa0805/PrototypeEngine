#include "InputSystem.h"
#include <SDL3/SDL.h>
#include <cstring>
#include "EngineWindow.h"

InputState InputSystem::mState;

SDL_Gamepad* InputSystem::mController = nullptr;

SDL_Window* InputSystem::mWindow = nullptr;

InputSystem::MouseMode InputSystem::mGameMouseMode = AbsoluteMouse;

InputSystem::MouseMode InputSystem::mMouseMode = AbsoluteMouse;

InputContext InputContextManager::sCurrentContext = InputContext::Engine;

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
    return mCurrState[keyCode] == 1;
}

bool KeyboardState::GetKeyDown(SDL_Scancode keyCode) const
{
	// 前の状態は押されていない & 現在押されている
	return mPrevState[keyCode] == 0 && mCurrState[keyCode] != 0;
}

bool KeyboardState::GetKeyUp(SDL_Scancode keyCode) const
{
	// 前の状態は押されていた & 現在押されていない
	return mPrevState[keyCode] != 0 && mCurrState[keyCode] == 0;
}

bool KeyboardState::GetKey(SDL_Scancode keyCode) const
{
	// 押し続けている（現在押されている）
	return mCurrState[keyCode] != 0;
}

bool KeyboardState::GetAnyKeyDown() const
{
	for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
	{
		if (mCurrState[i] != 0)
		{
			return true; // 何かのキーが押されている
		}
	}
	return false; // どのキーも押されていない
}

bool MouseState::GetButtonValue(int button) const
{
	return (SDL_BUTTON_MASK(button) & mCurrButtons);
}

ButtonState MouseState::GetButtonState(int button) const
{
	int mask = SDL_BUTTON_MASK(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mCurrButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mCurrButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool MouseState::GetButtonDown(int button) const
{
	int mask = SDL_BUTTON_MASK(button);
	return (mask & mPrevButtons) == 0 && (mask & mCurrButtons) != 0;
}

bool MouseState::GetButtonUp(int button) const
{
	int mask = SDL_BUTTON_MASK(button);
	return (mask & mPrevButtons) != 0 && (mask & mCurrButtons) == 0;
}

bool MouseState::GetButton(int button) const
{
	int mask = SDL_BUTTON_MASK(button);
	return (mask & mCurrButtons) != 0;
}

bool ControllerState::GetButtonValue(SDL_GamepadButton button) const
{
	return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GamepadButton button) const
{
	if (mPrevButtons[button] == 0)
	{
		if (mCurrButtons[button] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrButtons[button] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool ControllerState::GetButtonDown(SDL_GamepadButton button) const
{
	return mPrevButtons[button] == 0 && mCurrButtons[button] != 0;
}

bool ControllerState::GetButtonUp(SDL_GamepadButton button) const
{
	return mPrevButtons[button] != 0 && mCurrButtons[button] == 0;
}

bool ControllerState::GetButton(SDL_GamepadButton button) const
{
	return mCurrButtons[button] != 0;
}

bool InputSystem::Initialize()
{
	// Keyboard
	// Assign current state pointer
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// Clear previous state memory
	memset(mState.Keyboard.mPrevState, 0,
		SDL_SCANCODE_COUNT);

	// Mouse (just set everything to 0)
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mPrevButtons = 0;

	// Get the connected controller, if it exists
	mController = SDL_OpenGamepad(0);
	// Initialize controller state
	mState.Controller.mIsConnected = (mController != nullptr);
	memset(mState.Controller.mCurrButtons, 0,
		SDL_GAMEPAD_BUTTON_COUNT);
	memset(mState.Controller.mPrevButtons, 0,
		SDL_GAMEPAD_BUTTON_COUNT);

	return true;
}

void InputSystem::Shutdown()
{
	if (mController)
	{
		SDL_CloseGamepad(mController);
		mController = nullptr;
	}
}

void InputSystem::PrepareForUpdate()
{
	// Copy current state to previous
	// Keyboard
	memcpy(mState.Keyboard.mPrevState,
		mState.Keyboard.mCurrState,
		SDL_SCANCODE_COUNT);

	// Mouse
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	mState.Mouse.mIsRelative = false;
	mState.Mouse.mScrollWheel = Vector2::Zero;

	// Controller
	memcpy(mState.Controller.mPrevButtons,
		mState.Controller.mCurrButtons,
		SDL_GAMEPAD_BUTTON_COUNT);
}

void InputSystem::Update()
{
	// Mouse
	float x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mState.Mouse.mCurrButtons = SDL_GetMouseState(&x, &y);
	}


	if(mMouseMode == MouseMode::CaptureMouse)
	{
		SDL_WarpMouseInWindow(EngineWindow::GetRenderer()->GetWindow(), mState.Mouse.mMousePos.x, mState.Mouse.mMousePos.y);
	}
	else
	{
		mState.Mouse.mMousePos.x = static_cast<float>(x);
		mState.Mouse.mMousePos.y = static_cast<float>(y);
	}

	// Controller
	// Buttons
	for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++)
	{
		mState.Controller.mCurrButtons[i] =
			SDL_GetGamepadButton(mController,
				SDL_GamepadButton(i));
	}

	// Triggers
	mState.Controller.mLeftTrigger =
		Filter1D(SDL_GetGamepadAxis(mController,
			SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
	mState.Controller.mRightTrigger =
		Filter1D(SDL_GetGamepadAxis(mController,
			SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));

	// Sticks
	x = SDL_GetGamepadAxis(mController,
		SDL_GAMEPAD_AXIS_LEFTX);
	y = -SDL_GetGamepadAxis(mController,
		SDL_GAMEPAD_AXIS_LEFTY);
	mState.Controller.mLeftStick = Filter2D(x, y);

	x = SDL_GetGamepadAxis(mController,
		SDL_GAMEPAD_AXIS_RIGHTX);
	y = -SDL_GetGamepadAxis(mController,
		SDL_GAMEPAD_AXIS_RIGHTY);
	mState.Controller.mRightStick = Filter2D(x, y);
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_EVENT_MOUSE_WHEEL:
		mState.Mouse.mScrollWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y));
		break;
	default:
		break;
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	bool set = value ? true : false;
	SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(),set);

	mState.Mouse.mIsRelative = value;
}

float InputSystem::Filter1D(int input)
{
	// A value < dead zone is interpreted as 0%
	const int deadZone = 250;
	// A value > max value is interpreted as 100%
	const int maxValue = 30000;

	float retVal = 0.0f;

	// Take absolute value of input
	int absValue = input > 0 ? input : -input;
	// Ignore input within dead zone
	if (absValue > deadZone)
	{
		// Compute fractional value between dead zone and max value
		retVal = static_cast<float>(absValue - deadZone) /
			(maxValue - deadZone);
		// Make sure sign matches original value
		retVal = input > 0 ? retVal : -1.0f * retVal;
		// Clamp between -1.0f and 1.0f
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}

	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// Make into 2D vector
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// If length < deadZone, should be no input
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// Calculate fractional value between
		// dead zone and max value circles
		float f = (length - deadZone) / (maxValue - deadZone);
		// Clamp f between 0.0f and 1.0f
		f = Math::Clamp(f, 0.0f, 1.0f);
		// Normalize the vector, and then scale it to the
		// fractional value
		dir *= f / length;
	}

	return dir;
}

void InputSystem::SetGameMouseMode(MouseMode mode)
{
	if (mode == MouseMode::RelativeMouse)
	{
		SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), true);
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else if (mode == MouseMode::AbsoluteMouse)
	{
		SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), false);
	}
	else
	{
		SDL_Log("Unknown mouse mode");
	}
	mGameMouseMode = mode;
}

void InputSystem::SetMouseMode(MouseMode mode)
{
	if (mode == MouseMode::RelativeMouse)
	{
		SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), true);
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else if (mode == MouseMode::AbsoluteMouse)
	{
		SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), false);
	}
	else
	{
		SDL_Log("Unknown mouse mode");
	}
}

void InputSystem::RelativeMouseMode()
{
	mMouseMode = MouseMode::CaptureMouse;
	// --- 非表示にする瞬間 ---
	SDL_GetMouseState(&mState.Mouse.mMousePos.x, &mState.Mouse.mMousePos.y);   // 現在位置を保存
	SDL_HideCursor();                      // カーソル非表示
	// 相対モードON（移動量取得可）
	SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), true);
}

void InputSystem::AbsoluteMouseMode()
{
	mMouseMode = MouseMode::AbsoluteMouse;
	SDL_SetWindowRelativeMouseMode(EngineWindow::GetRenderer()->GetWindow(), false);// 相対モードOFF
	SDL_ShowCursor();                      // カーソル再表示
	SDL_WarpMouseInWindow(EngineWindow::GetRenderer()->GetWindow(), mState.Mouse.mMousePos.x, mState.Mouse.mMousePos.y);  // 元の位置に復元
}

void InputContextManager::SetContext(InputContext ctx)
{
	sCurrentContext = ctx;
	if(ctx == InputContext::Engine)
	{
		InputSystem::SetMouseMode(InputSystem::AbsoluteMouse);
	}
	else
	{
		InputSystem::SetMouseMode(InputSystem::GetGameMouseMode());
	}
}
