#include "HierarchyPanel.h"
#include "SceneManager.h" // SceneManager::GetNowScene() ���g�����߂ɕK�v
#include "Actor.h"        // new ActorObject() ���g�����߂ɕK�v

HierarchyPanel::HierarchyPanel(Renderer* renderer)
	:GUIPanel(renderer)
	, mSelectedActor(nullptr)
{
}

HierarchyPanel::~HierarchyPanel()
{
	if (mSelectedActor)
	{
		mSelectedActor = nullptr;
	}
}

void HierarchyPanel::Initialize(float width, float height, ImTextureRef ref)
{
	mWidthPos = width * 0.5f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.15f;
	mHeightSize = height - 55.0f;
}

void HierarchyPanel::Draw(float width, float height, ImTextureRef ref)
{
    if (isResetLayout)
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
		isResetLayout = false;
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
    }
	//  �V�����E�B���h�E�̍쐬
	if(ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
	{
		GUIPanelMenu();

		// ----------------------------------------------------------------
		// 1. ���݂̃V�[���̃A�N�^�[�ꗗ��\������
		// ----------------------------------------------------------------
		BaseScene* currentScene = SceneManager::GetNowScene();
		if (currentScene)
		{
			string name = currentScene->GetName();
			//���V�[������\��
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Scene:%s", currentScene->GetName().c_str());
			ImGui::Separator();
			// Scene����A�N�^�[���X�g���擾����֐����Ăяo��
			// BaseScene::GetActors() ���K�v
			const vector<ActorObject*>& actors = currentScene->GetActors();

			//�e�A�N�^�[�����[�v���ĕ\��
			for (auto& actor : actors)
			{
				//ImGui::Selectable()���g���ăA�N�^�[����\��
				//�I����Ԃ�mSelectedActor�Ɣ�r���Đݒ�
				bool isSelected = (mSelectedActor == actor);
				ImGui::PushID(actor);//�������O�̃A�N�^�[�����Ă����j�[�NID�ɂȂ�悤�ɂ���
				if (ImGui::Selectable(actor->GetName().c_str(), isSelected))
				{
					//2.�N���b�N���ꂽ��I�𒆂̃A�N�^�[���X�V
					mSelectedActor = actor;
				}
				ImGui::PopID();
			}

		}
		// ----------------------------------------------------------------
		// 3. �p�l���̋󂫃X�y�[�X���E�N���b�N�����Ƃ��̃��j���[
		// ----------------------------------------------------------------
		//ImGui::BeginPopupContextWindow() �́A���݂̃E�B���h�E���t�H�[�J�X����Ă����Ԃ�
        // �E�N���b�N���ꂽ�ꍇ�Ƀ|�b�v�A�b�v���j���[���J�n���܂��B
		if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Create Empty Actor"))
			{
				//3.ActorObject�̐����ƃV�[���ւ̒ǉ�

				// ActorObject::ActorObject() �R���X�g���N�^���ňȉ��̏������s���Ă���O��ł��B
				// 1. SceneManager::GetNowScene() ���擾�� mGame �ɐݒ�
				// 2. mGame->AddActor(this); ���Ăяo���A���݂̃V�[���� Actor ���X�g�ɒǉ�
				ActorObject* newActor = new ActorObject();
				mSelectedActor = newActor; // �V����������A�N�^�[�������őI��

				// (���O�o��)
				// Debug::Log("Created new Actor: %s\n", newActor->GetName().c_str());
			}

			//(�I�v�V����)
			if (ImGui::MenuItem("Create Mesh Actor"))
			{
				//TODO: ���b�V���A�N�^�[�쐬����
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void HierarchyPanel::ClearPointer()
{
	mSelectedActor = nullptr;
}
