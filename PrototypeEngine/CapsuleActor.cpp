#include "CapsuleActor.h"

CapsuleActor::CapsuleActor()
	:ActorObject()
{
	MeshRenderer* mc = new MeshRenderer(this);
	vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs("Capsule.fbx");
	mc->SetMeshs(mesh);

	// Add collision Sphere
	mCapsule = new CapsuleCollider(this);
	Capsule capsule(Vector3(0,1.0f,0), Vector3(0, -1.0f, 0), 0.5f);
	mCapsule->SetObjectCapsule(capsule);

}