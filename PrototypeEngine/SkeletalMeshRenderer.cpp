#include "SkeletalMeshRenderer.h"


SkeletalMeshRenderer::SkeletalMeshRenderer(ActorObject* owner)
	:MeshRenderer(owner, true)
	, mSkeleton(nullptr)
	, mAnimator(nullptr)
{
	mName = "SkeletalMeshRenderer";
}

SkeletalMeshRenderer::~SkeletalMeshRenderer()
{
}

void SkeletalMeshRenderer::Draw(Shader* shader)
{
	for (unsigned int i = 0; i < mMeshs.size(); i++) 
	{
		for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++) 
		{
			if (mMeshs[i])
			{
				// Set the world transform
				shader->SetMatrixUniform("uWorldTransform",
					mOwner->GetWorldTransform());
				if (mAnimator)
				{
					// Set the matrix palette
					shader->SetMatrixUniforms("uMatrixPalette",
						&mAnimator->GetPalette().mEntry[0],
						SkeletonLayout::MAX_SKELETON_BONES);
				}
				Texture* t = nullptr;
				// Set the active texture
				t = mMeshs[i]->GetTexture(j);
				if (t)
				{
					t->SetActive();
				}
				else 
				{
					shader->SetNoTexture();
				}
				MaterialInfo m = mMeshs[i]->GetMaterialInfo()[j];

				// �s�����x�ɂ���ău�����h�ݒ�i1�񂾂��ōςނȂ烋�[�v�̊O�ł�OK�j
				if (m.Color.w < 1.0f)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthMask(GL_FALSE);  // �������̂͐[�x�������ݖ����i�C�Ӂj
				}
				else
				{
					glDisable(GL_BLEND);
					glDepthMask(GL_TRUE);   // �s�������̂͒ʏ�ʂ�
				}
				//�}�e���A���̐F��ݒ�
				shader->SetColorUniform("uTexture", m);
				// ���b�V���̒��_�z����A�N�e�B�u�ɐݒ肵�܂�
				VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
				va->SetActive();
				// �`��
				glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
			}
		}
	}
}

void SkeletalMeshRenderer::DrawForShadowMap(Shader* shader)
{
	for (unsigned int i = 0; i < mMeshs.size(); i++)
	{
		for (unsigned int j = 0; j < mMeshs[i]->GetVertexArrays().size(); j++)
		{
			if (mMeshs[i])
			{
				// Set the world transform
				shader->SetMatrixUniform("uWorldTransform",
					mOwner->GetWorldTransform());
				if (mAnimator)
				{
					// Set the matrix palette
					shader->SetMatrixUniforms("uMatrixPalette",
						&mAnimator->GetPalette().mEntry[0],
						SkeletonLayout::MAX_SKELETON_BONES);
				}

				// �u�����h�Ȃǂ̓V���h�E�}�b�v�`�掞�͈�ؕs�v
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);

				// ���b�V���̒��_�z����A�N�e�B�u�ɐݒ肵�܂�
				VertexArray* va = mMeshs[i]->GetVertexArrays()[j];
				va->SetActive();
				// �`��
				glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
			}
		}
	}
}

void SkeletalMeshRenderer::Update(float deltaTime)
{
	/*
	if (!mAnimator) { return; }
	mAnimator->Update(deltaTime);
	*/
}
void SkeletalMeshRenderer::LoadSkeletonMesh(const string& fileName, ActorObject* actor)
{
	const vector<class Mesh*>& mesh = EngineWindow::GetRenderer()->GetMeshs(fileName);
	mMeshs.insert(mMeshs.end(), mesh.begin(), mesh.end());

	Skeleton* sk = mGame->GetSkeleton(fileName);
	mSkeleton = sk;
	if (mSkeleton != nullptr)
	{
		mSkeleton->SetParentActor(actor);
	}
	mIsSkeletal = true;
}

void SkeletalMeshRenderer::SetSkeleton(Skeleton* sk, ActorObject* actor)
{
	mSkeleton = nullptr;
	mSkeleton = sk;


	if (!mSkeleton) { return; }

	const auto& bones = mSkeleton->GetBones();

	//1.mParentIndex ���g���Đe�q�֌W���\�z
	for (size_t i = 0; i < bones.size(); i++)
	{
		const Skeleton::Bone& bone = bones[i];
		BoneActor* childActor = mSkeleton->GetBoneActor()[i];

		//mParentIndex�͐e�{�[���̃C���f�b�N�X
		if (bone.mParent != -1)
		{
			// �e�{�[����Actor���擾
			BoneActor* parentActor = mSkeleton->GetBoneActor()[bone.mParent];

			// Transform::AddChildActor() ���Ăяo���Đe�q�֌W������
			// Transform.cpp �Ɏ�������Ă��� AddChildActor �𗘗p
			parentActor->AddChildActor(childActor);

			// (�I�v�V����) SkeletalMeshRenderer�̃I�[�i�[Actor��e�ɂ���ꍇ
			// childActor->SetParentActor(this->GetOwner());
		}
		else
		{
			//���[�g�{�[���̏ꍇ
			//���[�g�A�N�^�[��SkeletonMeshRenderer�����I�[�i�[�̃I�u�W�F�N�g�̎q�ɂ���
			if (mOwner)
			{
				mOwner->AddChildActor(childActor);
			}
		}
		/*
		// ���[�J���o�C���h�|�[�Y��ݒ�
		// BoneActor��Transform�Ƀ{�[���̃��[�J���ʒu�Ɖ�]��ݒ肵�A
		// �{�[���I�t�Z�b�g�iBindPose�j�𔽉f�����܂��B
		Matrix4 bindPose = bone.mLocalBindPose;
		childActor->SetLocalPosition(bindPose.GetTranslation());
		childActor->SetLocalRotation(bindPose.GetRotation());
		childActor->SetLocalScale(bindPose.GetScale());
		*/
	}
}

void SkeletalMeshRenderer::SetAnimator(Animator* animator)
{
	mAnimator = nullptr;
	mAnimator = animator;
}

void SkeletalMeshRenderer::Serialize(json& j) const
{
	// 1. �x�[�X�N���X (MeshRenderer) �̃V���A���C�Y���Ăяo��
	//    -> ����ɂ��AmMeshFilePath �₻�̑��̊�{�v���p�e�B���������܂��
	MeshRenderer::Serialize(j);

	// 2. �R���|�[�l���g�̌^���uSkeletalMeshRenderer�v�ŏ㏑��
	//    -> ActorObject::Deserialize()�̃t�@�N�g���[�����ŁA���̌^���g����
	//       SkeletalMeshRenderer�̃C���X�^���X�����������悤�ɂ���
	//j["Type"] = "SkeletalMeshRenderer";

	// 3. (�K�v�ɉ�����) �X�P���^�����b�V���ŗL�̃v���p�e�B��ǋL
	//    mAnimator�͒ʏ�AActor�̕ʃR���|�[�l���g�Ƃ��ăV���A���C�Y����邽�߁A�����ł͏ȗ�
}

void SkeletalMeshRenderer::Deserialize(const json& j)
{
	// 1. �x�[�X�N���X�̃f�V���A���C�Y���Ăяo�� (MeshRenderer::Deserialize)
	//    -> JSON���� mMeshFilePath ��ǂݍ��݁AEngineWindow::GetRenderer()->GetMeshs() ���Ăяo����
	//       mMeshs (���b�V��) �̃��[�h�Ɛݒ������������B
	MeshRenderer::Deserialize(j);

	// 2. �x�[�X�N���X�œǂݍ��܂ꂽ�t�@�C���p�X (mMeshFilePath) ���g���ăX�P���g�������[�h����
	std::string fileName = GetMeshFilePath(); // GetMeshFilePath() �� mMeshFilePath ��Ԃ��O��

	// SkeletalMeshRenderer::LoadSkeletonMesh �̃X�P���g�����[�h�����̃��W�b�N
	Skeleton* sk = mOwner->GetGame()->GetSkeleton(fileName);
	mSkeleton = sk;
	/*
	if (mSkeleton != nullptr)
	{
		mSkeleton->SetParentActor(mOwner); // Actor�ɃX�P���g����ݒ�
	}
	*/

	mIsSkeletal = true; // �X�P���g���������Ă��邱�Ƃ𖾎�

	// 3. �A�j���[�^�[�̃����N�iAnimator���ʃR���|�[�l���g�̏ꍇ�j
	//    ActorObject�ɃA�^�b�`����Ă���Animator�R���|�[�l���g���擾���AmAnimator�ɐݒ肵�܂��B
	//    (��: Animator�N���X������O��)
	//    Animator* animator = mOwner->GetComponent<Animator>(); 
	//    SetAnimator(animator);
}

void SkeletalMeshRenderer::DrawGUI()
{
	//MeshRenderer�̃v���p�e�B
	ImGui::Text("SkeletalMeshRenderer");

	//1.�t�@�C���p�X�̎擾
	string currentPath = mFilePath;
	static char pathBuffer[256];
	strncpy_s(pathBuffer, currentPath.c_str(), sizeof(pathBuffer));
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';

	//2.�t�@�C���p�X�̓��̓t�B�[���h
	ImGui::InputText("Mesh File Path", pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_ReadOnly);

	//3.�t�@�C�����[�h�{�^��(�����Ńt�@�C���I��UI���J�����AProjectPanel�����Drag&Drop��z��)
	//Drag&Drop�z��
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			//�y�C���[�h���t�@�C���p�X�ł���Ɖ���
			const char* dropPath = (const char*)payload->Data;
			string path = StringConverter::ExtensionFileName(dropPath);
			//�t�@�C���p�X���g�����[�h�������Ăяo��
			vector<class Mesh*> mesh = EngineWindow::GetRenderer()->GetMeshs(path);
			SetMeshs(mesh);
			mFilePath = path;

			Skeleton* sk = mGame->GetSkeleton(path);
			mSkeleton = sk;
			//�{�[���̐e�q�֌W���\�z
			if (mSkeleton != nullptr)
			{
				SetSkeleton(mSkeleton, mOwner);
			}
			mIsSkeletal = true;
		}
		ImGui::EndDragDropTarget();
	}
	/*
	// �{�^���N���b�N�Ńt�@�C���I���_�C�A���O���J������
	if (ImGui::Button("Load Mesh from File"))
	{
		// �O���̃t�@�C���I���_�C�A���O (��: nativefiledialog) ���J���A
		// �I�����ꂽ�t�@�C���p�X�� meshRenderer->Load(...) �ɓn���B
	}
	*/

	ImGui::Separator();
}
