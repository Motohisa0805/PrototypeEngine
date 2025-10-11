#include "CubeActor.h"

CubeActor::CubeActor()
	:ActorObject()
{
	mName = "CubeActor";
	MeshRenderer* mc = new MeshRenderer(this);
	vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs("Cube.fbx");
	mc->AddMeshs(mesh);


	BoxCollider* box = new BoxCollider(this);
	box->SetObjectOBB(mc->GetMeshs()[0]->GetOBBBoxs()[0]);
	mBox = box;
}
