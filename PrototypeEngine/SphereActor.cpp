#include "SphereActor.h"

SphereActor::SphereActor()
	:ActorObject()
{
	mName = "SphereActor";
	MeshRenderer* mc = new MeshRenderer(this);
	vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs("Sphere.fbx");
	mc->AddMeshs(mesh);
	// Add collision Sphere
	mSphere = new SphereCollider(this);
	Sphere sphere(mLocalPosition, 0.5f);
	mSphere->SetObjectSphere(sphere);
}
