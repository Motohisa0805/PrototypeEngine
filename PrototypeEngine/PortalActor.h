#pragma once
#include "MeshActor.h"
#include "DialogBox.h"
#include "BoxCollider.h"

class PortalActor: public ActorObject
{
private:
	MeshActor*		mMeshActor;

	BoxCollider*	mBoxCollider;

	float			mMeshAlpha;
public:
	PortalActor();
	~PortalActor();

	void			RelativeMouseMode(bool relative);
	void			UpdateActor(float deltaTime) override;

	void			OnCollisionEnter(ActorObject* other) override;
};

