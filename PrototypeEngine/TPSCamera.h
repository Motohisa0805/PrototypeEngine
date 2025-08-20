#pragma once
#include "Actor.h"
#include "FollowCamera.h"

class TPSCamera : public ActorObject
{
private:
	class FollowCamera* mFollowCamera;
public:
	TPSCamera();
	class FollowCamera* GetFollowCamera() { return mFollowCamera; }
};

