#include "Skeleton.h"


Skeleton::~Skeleton()
{
}
//�t�@�C���`���œǂݍ��݊֐���ύX
bool Skeleton::Load(const string& fileName)
{
	// �t�@�C���̊g���q���擾
	string extension = fileName.substr(fileName.find_last_of('.') + 1);

	// **FBX �̏ꍇ**
	if (extension == "fbx")
	{
		return LoadFromFBX(fileName);
	}

	return false;
}
//�o�C�i������̓ǂݍ���
bool Skeleton::LoadFromSkeletonBin(const string& fileName)
{
	string name = StringConverter::RemoveString(fileName, File_P::ModelPath);
	name = StringConverter::RemoveExtension(name);
	std::ifstream in(File_P::BinaryFilePath + name + File_P::BinarySkelPath, std::ios::binary);
	if (!in)
	{
		SDL_Log("Failed to open Skeleton bin: %s", fileName.c_str());
		return false;
	}

	uint32_t boneCount = 0;
	in.read((char*)&boneCount, sizeof(uint32_t));

	if (boneCount > SkeletonLayout::MAX_SKELETON_BONES)
	{
		SDL_Log("Skeleton bin exceeds max bones");
		return false;
	}

	mBones.clear();
	mBones.reserve(boneCount);

	for (uint32_t i = 0; i < boneCount; ++i)
	{
		SkeletonBinBone bin{};
		in.read((char*)&bin, sizeof(SkeletonBinBone));

		Bone b;
		b.mName = bin.name;
		b.mShortName = bin.shortName;
		b.mParent = bin.parentIndex;
		b.mLocalBindPose.SetPosition(bin.position);
		b.mLocalBindPose.SetRotation(bin.rotation);
		b.mLocalBindPose.SetScale(bin.scale);

		//boneNameToIndex�Ƀ{�[�������L�[�Ƀ{�[���ԍ����i�[
		mBoneNameToIndex[b.mName] = static_cast<int>(mBones.size());
		//������mBoneTransform�Ƀ{�[���̔ԍ����i�[
		mBoneTransform[b.mShortName] = static_cast<int>(mBones.size());

		//�{�[���x�N�^�[�Ɋi�[
		mBones.push_back(b);

		//assimp�ł̓I�t�Z�b�g�s������̂܂ܗ��p
		mGlobalInvBindPoses.push_back(b.mLocalBindPose.ToMatrix());


		//�{�[���̃I�u�W�F�N�g�𐶐�
		BoneActor* boneActor = new BoneActor();
		boneActor->SetBoneIndex(static_cast<int>(mBones.size()));
		boneActor->SetBoneName(b.mShortName);
		mBoneActors.push_back(boneActor);
	}

	ComputeGlobalInvBindPose();
	return true;
}


bool Skeleton::LoadFromFBX(const string& fileName)
{
	//�t�@�C���ǂݍ���
	Assimp::Importer importer;
	//�O�p�`�Ń|���S�����擾�A�{�[���̃E�F�C�g���ő�4�ɐ����A�X�P�[�����O��1unit��
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_Triangulate | aiProcess_LimitBoneWeights |
		aiProcess_GlobalScale | aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder);
	//���f�������邩�m�F
	if (!scene || !scene->HasMeshes()) {
		//�Ȃ��Ȃ�G���[���b�Z�[�W
		SDL_Log("Failed to load FBX: %s", importer.GetErrorString());
		return false;
	}
	//�{�[���̏�����
	mBones.clear();
	//���b�V���̐���for��
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) 
	{
		aiMesh* mesh = scene->mMeshes[i];
		//���b�V�����̃{�[������for��
		for (unsigned int j = 0; j < mesh->mNumBones; j++) 
		{
			//�{�[�����擾
			aiBone* bone = mesh->mBones[j];
			//�{�[�������擾
			string boneName = bone->mName.C_Str();
			//�擾�����{�[��������Z�k�������O�ɕϊ��������̂��擾
			string boneShortName = ConvertSimpleBoneName(bone->mName.C_Str());
			//boneNameToIndex�ɂ��łɓ����{�[�����Ȃ����`�F�b�N
			if (mBoneNameToIndex.find(boneName) != mBoneNameToIndex.end()) continue;
			//�{�[���̍\����
			Bone boneStruct;
			//�{�[����mOffsetMatrix��vector�Ɋi�[
			mOffsetMatrix.push_back(bone->mOffsetMatrix);
			//�{�[���{���̖��O����
			boneStruct.mName = boneName;
			//�{�[���̒Z�k������
			boneStruct.mShortName = boneShortName;
			// ��� SetParentBones() �Őݒ肷��
			boneStruct.mParent = -1;

			// �o�C���h�|�[�Y�̕ϊ�
			//�{�[����mOffsetMatrix�擾
			aiMatrix4x4 bindPose = bone->mOffsetMatrix;
			aiVector3D pos;
			aiQuaternion rot;
			aiVector3D scale;
			//�{�[���̃o�C���h�|�[�Y���e�l�ɕ���
			bindPose.Decompose(scale, rot, pos);

			//rot.x = -rot.x;
			//pos.x = -pos.x;
			//���[�J���̃o�C���h�|�[�Y�ɉ�]�A���s�ړ��A�X�P�[�����O���i�[
			boneStruct.mLocalBindPose.SetRotation(Quaternion(rot.x, rot.y, rot.z, rot.w));
			boneStruct.mLocalBindPose.SetPosition(Vector3(pos.x, pos.y, pos.z));
			boneStruct.mLocalBindPose.SetScale(Vector3(scale.x, scale.y, scale.z));
			//boneNameToIndex�Ƀ{�[�������L�[�Ƀ{�[���ԍ����i�[
			mBoneNameToIndex[boneName] = static_cast<int>(mBones.size());
			//������mBoneTransform�Ƀ{�[���̔ԍ����i�[
			mBoneTransform[boneStruct.mShortName] = static_cast<int>(mBones.size());
			//�{�[���x�N�^�[�Ɋi�[
			mBones.push_back(boneStruct);

			//assimp�ł̓I�t�Z�b�g�s������̂܂ܗ��p
			mGlobalInvBindPoses.push_back(boneStruct.mLocalBindPose.ToMatrix());
			//�{�[���̃I�u�W�F�N�g�𐶐�
			BoneActor* boneActor = new BoneActor();
			boneActor->SetBoneIndex(static_cast<int>(mBones.size()));
			boneActor->SetBoneName(boneShortName);
			mBoneActors.push_back(boneActor);
		}
	}

	// �e�q�֌W��ݒ�
	SetParentBones(scene->mRootNode, -1);

	//fileName����Path����������菜��
	string result = StringConverter::RemoveString(fileName, File_P::ModelPath);
	result = StringConverter::RemoveExtension(result);
	std::ofstream out(File_P::BinaryFilePath + result + File_P::BinarySkelPath, std::ios::binary);
	if (!out)
	{
		SDL_Log("Failed to open skelbin for writing.");
		return false;
	}

	uint32_t boneCount = static_cast<uint32_t>(mBones.size());
	out.write((char*)&boneCount, sizeof(uint32_t));

	for (Bone& b : mBones)
	{
		SkeletonBinBone bin{};
		strncpy_s(bin.name, b.mName.c_str(), SkeletonLayout::MAX_SKELETONBINBONE);
		strncpy_s(bin.shortName, b.mShortName.c_str(), SkeletonLayout::MAX_SKELETONBINBONE);
		bin.parentIndex = b.mParent;
		bin.position = b.mLocalBindPose.GetPosition();
		bin.rotation = b.mLocalBindPose.GetRotation();
		bin.scale = b.mLocalBindPose.GetScale();

		out.write((char*)&bin, sizeof(SkeletonBinBone));
	}

	return true;
}

void Skeleton::SetParentBones(aiNode* node, int parentIndex)
{
	//�s���ȃ{�[���̏ꍇ�Ɏ��ɂ��̂܂܍ċN���邽�߂̏�����ǉ�
	string nodeName = node->mName.data;
	int nextIndex = parentIndex;

	// ���̃m�[�h���{�[���Ƃ��ēo�^����Ă��邩�m�F
	if (mBoneNameToIndex.find(nodeName) != mBoneNameToIndex.end()) 
	{
		int boneIndex = mBoneNameToIndex[nodeName];
		mBones[boneIndex].mParent = parentIndex;
		mBoneActors[boneIndex]->SetParentIndex(parentIndex);
		nextIndex = boneIndex;

		//�o�C���h�|�[�Y�����[�J�����ɕϊ�
		aiMatrix4x4 localMatrix = mOffsetMatrix[boneIndex];
		localMatrix = localMatrix.Inverse();

		if (parentIndex >= 0)
		{
			aiMatrix4x4 parentMatrixInv = mOffsetMatrix[parentIndex];
			localMatrix = parentMatrixInv * localMatrix;
		}

		aiVector3D pos;
		aiQuaternion rot;
		aiVector3D scale;
		localMatrix.Decompose(scale, rot, pos);

		mBones[boneIndex].mLocalBindPose.SetRotation(Quaternion(rot.x, rot.y, rot.z, rot.w));
		mBones[boneIndex].mLocalBindPose.SetPosition(Vector3(pos.x, pos.y, pos.z));
		mBones[boneIndex].mLocalBindPose.SetScale(Vector3(scale.x, scale.y, scale.z));
	}

	// �q�m�[�h���ċA�I�ɏ���
	for (unsigned int i = 0; i < node->mNumChildren; i++) 
	{
		SetParentBones(node->mChildren[i], nextIndex);
	}
}

string Skeleton::ConvertSimpleBoneName(string boneName)
{
	string bone = boneName;
	// �e�{�[���̕��ʂ̖��O�̔z��
	vector<string> boneNames = {
		"Hips","Spine","Chest","Neck","Head",
		"LeftShoulder","LeftArm","LeftForeArm","LeftHand",
		"RightShoulder","RightArm","RightForeArm","RightHand",
		"LeftUpLeg","LeftLeg","LeftFoot",
		"RightUpLeg","RightLeg","RightFoot",
	};
	for (string bonename : boneNames) 
	{
		if (bone.find(bonename) != string::npos && EndsWith(bone, bonename)) 
		{
			bone = bonename;
		}
	}
	return bone;
}

Matrix4 Skeleton::GetBonePosition(string boneName)
{
	Matrix4 boneMatrix;
	int index = 0;
	if (mBoneTransform.find(boneName) != mBoneTransform.end())
	{
		index = mBoneTransform[boneName];
	}
	boneMatrix = mGlobalCurrentPoses[index];
	
	return boneMatrix;
}

void Skeleton::AddBoneChildActor(string boneName, class ActorObject* actor)
{
	int index = 0;
	if (mBoneTransform.find(boneName) != mBoneTransform.end())
	{
		index = mBoneTransform[boneName];
	}
	mBoneActors[index]->AddChildActor(actor);
}

void Skeleton::SetParentActor(ActorObject* parent)
{
	for (unsigned int i = 0; i < mBoneActors.size(); i++)
	{
		mBoneActors[i]->AddParentActor(parent);
	}
}

void Skeleton::ComputeGlobalInvBindPose()
{
	// Resize to number of bones, which automatically fills identity
	mGlobalInvBindPoses.resize(GetNumBones());

	// Step 1: Compute global bind pose for each bone

	// The global bind pose for root is just the local bind pose
	mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose.ToMatrix();

	// Each remaining bone's global bind pose is its local pose
	// multiplied by the parent's global bind pose
	for (size_t i = 1; i < mGlobalInvBindPoses.size(); i++)
	{
		Matrix4 localMat = mBones[i].mLocalBindPose.ToMatrix();
		mGlobalInvBindPoses[i] = localMat * mGlobalInvBindPoses[mBones[i].mParent];
	}

	// Step 2: Invert
	for (size_t i = 0; i < mGlobalInvBindPoses.size(); i++)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}