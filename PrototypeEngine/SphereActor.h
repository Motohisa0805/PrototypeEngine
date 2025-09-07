#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "SphereCollider.h"
#include "Mesh.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//���^�̃I�u�W�F�N�g
//Unity�̂悤�Ƀf�t�H���g�ō쐬�\�I�u�W�F�N�g
class SphereActor : public ActorObject
{
private:
	//���̃R���C�_�[
	SphereCollider*			mSphere;
public:
							SphereActor();

	SphereCollider*			GetSphere() { return mSphere; }
};

