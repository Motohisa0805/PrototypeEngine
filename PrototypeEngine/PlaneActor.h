#pragma once
#include "Actor.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "Mesh.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//���ʌ^�̃I�u�W�F�N�g
//Unity�̂悤�Ƀf�t�H���g�ō쐬�\�I�u�W�F�N�g
class PlaneActor : public ActorObject
{
private:
	//AABB�̃R���C�_�[
	vector<BoxCollider*>		mBoxs;

	MeshRenderer*				mMeshComp;
public:
								PlaneActor();
	vector<BoxCollider*>		GetBoxs() { return mBoxs; }
};