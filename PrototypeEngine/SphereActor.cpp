#include "SphereActor.h"
#include "EngineWindow.h"
#include "Renderer.h"
#include "MeshRenderer.h"
#include "SphereCollider.h"
#include "Mesh.h"

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
