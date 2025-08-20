#include "WindowRenderProperty.h"

float WindowRenderProperty::mWidth = 1920.0f;

float WindowRenderProperty::mHeight = 1080.0f;

Vector3 WindowRenderProperty::mViewEye = Vector3::Zero;

Vector3 WindowRenderProperty::mViewTarget = Vector3::UnitX;

Vector3 WindowRenderProperty::mViewUp = Vector3::UnitZ;

float WindowRenderProperty::mFieldOfView = 70.0f;

float WindowRenderProperty::mCameraNear = 0.1f;

float WindowRenderProperty::mCameraFar = 10000.0f;


void WindowRenderProperty::SetWindowScale(float width, float height)
{
	mWidth = width;
	mHeight = height;
}

void WindowRenderProperty::SetViewEye(Vector3 eye)
{
	mViewEye = eye;
}

void WindowRenderProperty::SetViewTarget(Vector3 target)
{
	mViewTarget = target;
}

void WindowRenderProperty::SetViewUp(Vector3 up)
{
	mViewUp = up;
}

void WindowRenderProperty::SetFieldOfView(float fieldOfView)
{
	mFieldOfView = fieldOfView;
}

void WindowRenderProperty::SetCameraNear(float cameraNear)
{
	mCameraNear = cameraNear;
}

void WindowRenderProperty::SetCameraFar(float cameraFar)
{
	mCameraFar = cameraFar;
}
