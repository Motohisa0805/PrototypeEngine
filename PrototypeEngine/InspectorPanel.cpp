#include "InspectorPanel.h"
#include "HierarchyPanel.h"//GetSelectedActor()���g�����߂ɕK�v

#include "Actor.h"
#include "Math.h"//Vector3,Quaternion���g�����߂ɕK�v

// ��: �O������ HierarchyPanel �̃C���X�^���X���擾�ł���֐�������Ɖ���

InspectorPanel::InspectorPanel(Renderer* renderer)
	:GUIPanel(renderer)
{
}

InspectorPanel::~InspectorPanel()
{

}

void InspectorPanel::Initialize(float width, float height, ImTextureRef ref)
{	
	mWidthPos = width * 0.8f;
	mHeightPos = 55.0f;
	mWidthSize = width * 0.2f;
	mHeightSize = height - 55.0f;
}

void InspectorPanel::Draw(float width, float height, ImTextureRef ref)
{
	if (isResetLayout)
	{
		// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos));
		ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize));
		isResetLayout = false;
	}
	else
	{
		// �E�C���h�E�ʒu�ƃT�C�Y���Œ�
		ImGui::SetNextWindowPos(ImVec2(mWidthPos, mHeightPos), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(mWidthSize, mHeightSize), ImGuiCond_Once);
	}
	//  �V�����E�B���h�E�̍쐬
	if(ImGui::Begin("SelectItem", nullptr, ImGuiWindowFlags_NoCollapse))
	{
		GUIPanelMenu();

		//1.�I�𒆂�Actor���擾
		ActorObject* selectedActor = GUIWinMain::GetHierarchyPanel()->GetSelectedActor();

		if(selectedActor)
		{
			// ----------------------------------------------------------------
			// 1.Actor�̖��O��\���E�ҏW
			// ----------------------------------------------------------------
			// C++ std::string�𒼐� ImGui::InputText �ɓn�����߂̏������K�v
			// �����ł͊ȈՓI�ɁAActor::mName �����J�����o�[�Ƃ��Ĉ���

			//Actor���̕ҏW(�ꎞ�o�b�t�@���g���̈��S�����ǁA�����ł͊ȗ���)
			char nameBuffer[128];
			strncpy_s(nameBuffer, selectedActor->GetName().c_str(), sizeof(nameBuffer) - 1);
			nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // �O�̂��߃k���I�[

			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
			{
				selectedActor->SetName(string(nameBuffer));
			}

			ImGui::Separator();

			// ----------------------------------------------------------------
			//2.Transform�R���|�[�l���g�̕\���E�ҏW(Transform��ActorObject�̊��N���X)
			// ----------------------------------------------------------------
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				//Position(Vector3)�̕ҏW
				Vector3 pos = selectedActor->GetLocalPosition();
				if (ImGui::DragFloat3("Position", &pos.x, 0.1f))//0.1f�̓h���b�O�̊��x
				{
					//���[�J���֐��Ȃ̂Œ���
					selectedActor->SetLocalPosition(pos);
				}
				//��]�������[�J���Ŏ擾
				//���[�J���Ȃ�X�P�[���l���܂܂Ȃ�����
				Vector3 eulerRad = selectedActor->GetLocalRotation().ToEulerAngles();
				Vector3 rot;
				rot.x = Math::ToDegrees(eulerRad.x);
				rot.y = Math::ToDegrees(eulerRad.y);
				rot.z = Math::ToDegrees(eulerRad.z);
				//�x���@�ŕ\���E�ҏW
				if (ImGui::DragFloat3("Rotation(deg)", &rot.x, 1.0f))
				{

					// ���W�A���ɕϊ����ĕۑ�
					Quaternion qx = Quaternion::CreateFromAxisAngle(Vector3::UnitX, rot.x);
					Quaternion qy = Quaternion::CreateFromAxisAngle(Vector3::UnitY, rot.y);
					Quaternion qz = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, rot.z);
					Quaternion newRotation = qy * qx * qz; // ZYX���ŉ�]��K�p
					selectedActor->SetLocalRotation(newRotation);
				}

				//Scale(Vector3)�̕ҏW
				Vector3 scale = selectedActor->GetLocalScale();
				if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))//0.1f�̓h���b�O�̊��x
				{
					//���[�J���֐��Ȃ̂Œ���
					selectedActor->SetLocalScale(scale);
				}
			}

			//----------------------------------------------------------------
			// �R���|�[�l���g�̃��X�g�\��
			//----------------------------------------------------------------
			const vector<Component*>& components = selectedActor->GetComponents();

			// �폜�Ώۂ̃R���|�[�l���g��ێ�����|�C���^
			Component* compToDelete = nullptr;
			for (Component* comp : components)
			{
				
				// �e�R���|�[�l���g�̃v���p�e�B�ҏWUI
				if (ImGui::CollapsingHeader(comp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					//�����Ɋe�R���|�[�l���g�ŗL�̃v���p�e�B�ҏW���W�b�N������

					ImGui::Text("[Type: %s]", comp->GetName().c_str());
					ImGui::SameLine();

					comp->DrawGUI();

					//----------------------------------------------------------------
					// �R���|�[�l���g�폜�{�^��
					//----------------------------------------------------------------

					ImGui::SameLine();

					ImGui::PushID(comp); // comp �̃A�h���X���ꎞ�I��ID�X�^�b�N�ɒǉ�
					{
						// �폜�{�^���̏���
						if (comp->GetName() != "Transform" && ImGui::Button("Remove"))
						{
							compToDelete = comp;
						}
					}
					ImGui::PopID(); // ID�X�^�b�N���� comp �̃A�h���X���폜

				}
			}
			ImGui::Separator();

			//���[�v������ɍ폜�����s
			if (compToDelete)
			{
				selectedActor->RemoveComponent(compToDelete);
				delete compToDelete;
				compToDelete = nullptr;
			}

			// ----------------------------------------------------------------
			// 2. �V�K�R���|�[�l���g�̒ǉ��{�^��
			// ----------------------------------------------------------------
			//�E�B���h�E�S�̂ɍL����傫�ȃ{�^��
			if (ImGui::Button("Add Component", ImVec2(-1.0f, 0.0f)))
			{
				ImGui::OpenPopup("ComponentSelector");
			}

			if (ImGui::BeginPopup("ComponentSelector"))
			{
				//�t�@�N�g���[����o�^���ꂽ�R���|�[�l���g�����X�g���擾
				vector<string> componentNames = ComponentFactory::GetRegisteredComponentNames();

				for (const string& compName : componentNames)
				{
					//����Actor�ɃA�^�b�`����Ă���R���|�[�l���g�͕\�����Ȃ�(Transform�͏���)
					if (ImGui::MenuItem(compName.c_str()))
					{
						//�t�@�N�g���[���g���ăR���|�[�l���g�𐶐�
						Component* newComp = ComponentFactory::CreateComponent(compName, selectedActor);
						if (newComp)
						{
							selectedActor->AddComponent(newComp);
							// ���b�V���ƃR���C�_�[�̈ˑ��������������珈��
							selectedActor->OnComponentAdded(newComp); // �� ActorObject���Ŏ�������
						}
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
		}
		else
		{
			ImGui::Text("Select an Actor in the Hierarchy.");
		}
	}
	ImGui::End();
}
