#include "TPSCamera.h"

TPSCamera::TPSCamera()
	:ActorObject()
{
	mFollowCamera = new FollowCamera(this);
}
