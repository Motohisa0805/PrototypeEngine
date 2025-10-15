#pragma once
#include "GUIWinMain.h"
#include "GUIPanel.h"

//�Q�[���̃V�[�����̃I�u�W�F�N�g��`�悷��\��̃N���X
//�{�i�I�ȕ`�揈���͖�����
class HierarchyPanel : public GUIPanel
{
private:
	//�I�𒆂̃A�N�^�[��ێ�����|�C���^�[
	ActorObject* mSelectedActor;
public:
	const char* GetName()override { return "Hierarchy"; }
	HierarchyPanel(class Renderer* renderer);
	~HierarchyPanel();

	void		Initialize(float width, float height, ImTextureRef ref = nullptr)override;

	void		Draw(float width, float height, ImTextureRef ref = nullptr)override;

	void		DrawActorNode(ActorObject* actor);

	void		ClearPointer()override;

	//�O������I�𒆂�Actor���擾
	ActorObject* GetSelectedActor() const { return mSelectedActor; }
};

