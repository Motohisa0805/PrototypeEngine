#include "Mesh.h"
#include "Shader.h"

namespace
{
	union Vertex
	{
		float f;
		uint8_t b[4];
	};
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const string& fileName, Renderer* renderer, int index)
{
	// �t�@�C���̊g���q���擾
	string extension = fileName.substr(fileName.find_last_of('.') + 1);

	// **FBX �̏ꍇ**
	if (extension == "fbx")
	{
		return LoadFromFBX(fileName, renderer,index);
	}

	return false; 
}

int Mesh::CheckMeshIndex(const string& fileName, Renderer* renderer)
{
	int index = 0;
	std::ifstream fileCheck(fileName);
	if (!fileCheck) {
		SDL_Log("FBX file not found: %s", fileName.c_str());
		return false;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || !scene->HasMeshes())
	{
		SDL_Log("Assimp Error: %s", importer.GetErrorString());
		return false;
	}

	index = scene->mNumMeshes;

	return index;
}

bool Mesh::LoadFromMeshBin(const string& fileName, Renderer* renderer, int index)
{
	string name = StringConverter::RemoveExtension(fileName);

	//1:�o�C�i�����ɕϊ��������_�A�C���f�b�N�X�f�[�^��bin�t�@�C������擾
	string number = std::to_string(index);
	//fileName����Path����������菜��
	string result = StringConverter::RemoveString(name, File_P::ModelPath);

	string binaryFilePath = File_P::BinaryFilePath + result + number + File_P::BinaryPath;
	//�o�C�i���t�@�C���̊m�F
	std::ifstream in(binaryFilePath, std::ios::binary);
	if (!in) {
		SDL_Log("Failed to open mesh binary: %s", binaryFilePath.c_str());
		return false;
	}

	//�o�C�i���f�[�^�̍\���̐錾
	MeshBinHeader header;
	//�錾�����\���̂ɓǂݍ��񂾃t�@�C���̏���ǂݍ���
	in.read((char*)&header, sizeof(header));
	//Texture�̃^�C�v����
	VertexArray::Layout layout = (header.layoutType == 0) ?
		VertexArray::PosNormTex : VertexArray::PosNormSkinTex;

	// �v�f�����v�Z�i1���_�������Vertex���j
	size_t stride = (layout == VertexArray::PosNormTex) ? MeshLayout::MESH_VERTEXCOUNT : MeshLayout::SKINMESH_VERTEXCOUNT;
	std::vector<Vertex> vertices(header.vertexCount * stride);
	std::vector<uint32_t> indices(header.indexCount);

	in.read((char*)vertices.data(), sizeof(Vertex) * vertices.size());
	in.read((char*)indices.data(), sizeof(uint32_t) * indices.size());

	// ���S�ʒu�┼�a���ė��p�������ꍇ
	AABB box = AABB(Vector3::Infinity, Vector3::NegInfinity);
	box.mMin = header.min;
	box.mMax = header.max;

	// AABB�̒��S�ƃT�C�Y����OBB�����i��]�Ȃ��j
	Vector3 center = (box.mMin + box.mMax) * 0.5f;
	Vector3 extents = (box.mMax - box.mMin) * 0.5f;
	Quaternion rotation = Quaternion::Identity; // �����Ȃ�
	OBB obbBox = OBB(Vector3::Zero, Quaternion::Identity, Vector3::Zero);
	obbBox = OBB(center, rotation, extents);


	mBoxs.push_back(box); // AABB���S�ȂǂɎg����
	mOBBBoxs.push_back(obbBox);
	mRadiusArray.push_back(header.colliderRadius);

	VertexArray* va = new VertexArray(vertices.data(), header.vertexCount,
		layout, indices.data(), header.indexCount);
	mVertexArrays.push_back(va);

	//2:Assimp���g���ăt�@�C������e�N�X�`���ƃ}�e���A�������擾
	string assimpFilePath = name + ".fbx";
	//�t�@�C���`�F�b�N
	std::ifstream fileCheck(assimpFilePath);
	if (!fileCheck)
	{
		SDL_Log("FBX file not found: %s", assimpFilePath.c_str());
		return false;
	}
	//���f�����擾
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(assimpFilePath,
		aiProcess_Triangulate | aiProcess_FlipUVs |
		aiProcess_GenNormals | aiProcess_GlobalScale |
		aiProcess_MakeLeftHanded |
		aiProcess_FlipWindingOrder);
	//MeshCheck
	if (!scene || !scene->HasMeshes())
	{
		SDL_Log("Assimp Error: %s", importer.GetErrorString());
		return false;
	}

	aiMesh* mesh = scene->mMeshes[index];

	//�e�N�X�`���ƃ}�e���A���̏����擾
		//�e�N�X�`���ƃ}�e���A���̓ǂݍ���
	std::unordered_map<string, Texture*> loadedTextures;
	string texFile = "MaterialTexure.png";
	// ���b�V���Ɋ֘A�t����ꂽ�}�e���A�����擾
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiString texturePath;
	//�t�@�C����FBX�����邩
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
	{
		texFile = texturePath.C_Str();

		// ���ߍ��݃e�N�X�`�����ǂ����`�F�b�N
		if (texFile[0] == '.') {
			int embeddedIndex = std::atoi(texFile.c_str() + 1);
			if (embeddedIndex < scene->mNumTextures) {
				aiTexture* embeddedTex = scene->mTextures[embeddedIndex];

				Texture* newTex = new Texture();
				if (newTex->LoadFromAssimp(embeddedTex)) {
					loadedTextures[texFile] = newTex;
				}
				else {
					newTex->Unload();
					delete newTex;
				}
			}
		}
		// �ʏ�̊O���e�N�X�`��
		else {
			if (loadedTextures.find(texFile) == loadedTextures.end())
			{
				Texture* newTex = new Texture();
				if (newTex->Load(File_P::ModelTexturePath + texFile))
				{
					loadedTextures[texFile] = newTex;
				}
				else {
					newTex->Unload();
					delete newTex;
				}
			}
		}

		if (loadedTextures.find(texFile) != loadedTextures.end())
		{
			mTextures.push_back(loadedTextures[texFile]);
		}
	}
	//�Ȃ��Ȃ�}�e���A���p�̃e�N�X�`�����[�h
	else
	{
		//�}�e���A���p�̃e�N�X�`���擾
		if (loadedTextures.find(texFile) == loadedTextures.end())
		{
			Texture* newTex = new Texture();
			if (newTex->Load(File_P::ModelTexturePath + texFile))
			{
				loadedTextures[texFile] = newTex;
			}
			else {
				newTex->Unload();
				delete newTex;
			}
		}

		if (loadedTextures.find(texFile) != loadedTextures.end())
		{
			mTextures.push_back(loadedTextures[texFile]);
		}
	}

	MaterialInfo info
	{
			Vector4(0,0,0,0),
			Vector3(0,0,0),
			Vector3(0,0,0),
			Vector3(0,0,0),
			0
	};

	aiColor4D diffuseColor;
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
	{

		info.Color = Vector4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
	}

	// �g�U�F�iDiffuse Color�j�̎擾
	aiColor3D diffuse(1.0f, 1.0f, 1.0f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

	// �����iAmbient Color�j�̎擾
	aiColor3D ambient(0.2f, 0.2f, 0.2f);
	material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

	// ���ʔ��ˁiSpecular Color�j�̎擾
	aiColor3D specular(0.5f, 0.5f, 0.5f);
	material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

	// �V�F�[�_�[�ɒl�𑗂�iglUniform3f ���g�p�j
	info.Ambient = Vector3(ambient.r, ambient.g, ambient.b);
	info.Diffuse = Vector3(diffuse.r, diffuse.g, diffuse.b);
	info.Specular = Vector3(specular.r, specular.g, specular.b);

	mMaterialInfo.push_back(info);

	float shininess = 0.0f;
	if (scene->HasMaterials())
	{

		if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess))
		{
			// �f�t�H���g�l��ݒ�
			shininess = 100.0f;
		}
		shininess = shininess / 128.0f;

		info.Shininess = shininess;
	}

	//�ǂݍ��ݐ���
	return true;
}

//FBX�p�ǂݍ��݊֐�
bool Mesh::LoadFromFBX(const string& fileName, Renderer* renderer, int index)
{
	//�t�@�C���`�F�b�N
	std::ifstream fileCheck(fileName);
	if (!fileCheck) 
	{
		SDL_Log("FBX file not found: %s", fileName.c_str());
		return false;
	}
	//���f�����擾
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenNormals  |
		aiProcess_GlobalScale |
		aiProcess_MakeLeftHanded |
		aiProcess_FlipWindingOrder);
	//MeshCheck
	if (!scene || !scene->HasMeshes())
	{
		SDL_Log("Assimp Error: %s", importer.GetErrorString());
		return false;
	}

	//���b�V�������[�h
	aiMesh* mesh;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	float radius = 0.0f;
	AABB box = AABB(Vector3::Infinity, Vector3::NegInfinity);
	OBB obbBox = OBB(Vector3::Zero,Quaternion::Identity,Vector3::Zero);

	mesh = scene->mMeshes[index];

	//���_�f�[�^�̕ϊ�
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D pos = mesh->mVertices[i];
		aiVector3D norm = mesh->mNormals[i];

		aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0, 0, 0);


		Vector3 vertexPos(pos.x, pos.y, pos.z);
		radius = Math::Max(radius, vertexPos.LengthSq());
		box.UpdateMinMax(vertexPos);
		// AABB�̒��S�ƃT�C�Y����OBB�����i��]�Ȃ��j
		Vector3 center = (box.mMin + box.mMax) * 0.5f;
		Vector3 extents = (box.mMax - box.mMin) * 0.5f;
		Quaternion rotation = Quaternion::Identity; // �����Ȃ�

		obbBox = OBB(center, rotation, extents);

		Vertex v;
		v.f = pos.x; vertices.push_back(v);

		v.f = pos.y; vertices.push_back(v);

		v.f = pos.z; vertices.push_back(v);

		v.f = norm.x; vertices.push_back(v);

		v.f = norm.y; vertices.push_back(v);

		v.f = norm.z; vertices.push_back(v);

		//���_�ɃE�F�C�g�ǉ��A�ȈՂ̂��ߕ������b�V���̏ꍇ�ȂǕs������(�C���ς�)
		if (mesh->HasBones())
		{
			Vertex boneIndex = { 0 };
			Vertex weight[4] = { 0,0,0,0 };
			ai_real allWeight = 0.0f;
			for (unsigned int j = 0; j < mesh->mNumBones; j++) 
			{
				aiBone* bone = mesh->mBones[j];
				for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) 
				{
					if (bone->mWeights[weightIndex].mVertexId != i ||
						bone->mWeights[weightIndex].mWeight <= 0)
					{
						continue;
					}
					float weightValue = bone->mWeights[weightIndex].mWeight;
					//���݂̃E�F�C�g�����4�ɓ��邩����
					for (unsigned int w = 0; w < 4; w++)
					{
						if (weight[w].f < weightValue)
						{
							// �V�t�g���đ}���i���݂�w�ȍ~�����ɂ��炷�j
							for (int k = 3; k > w; k--)
							{
								weight[k] = weight[k - 1];
								boneIndex.b[k] = boneIndex.b[k - 1];
							}
							//�V�����l��}��
							weight[w].f = weightValue;
							boneIndex.b[w] = j;
							break;
						}
					}
					allWeight += bone->mWeights[weightIndex].mWeight;
				}
			}
			//�[�����Z��������K��
			if (allWeight > 0.0f) 
			{
				//�E�F�C�g�̍��v��1.0�ɐ��K���i�[�����Z�����)
				allWeight = 1.0f / allWeight;
				weight[0].f *= allWeight;
				weight[1].f *= allWeight;
				weight[2].f *= allWeight;
				weight[3].f *= allWeight;
			}

			//�E�F�C�g���Ƀ\�[�g
			for (int i = 0; i < 3; i++) 
			{
				for (int j = i + 1; j < 4; j++) 
				{
					if (weight[i].f < weight[j].f) 
					{
						std::swap(weight[i], weight[j]);
						std::swap(boneIndex.b[i], boneIndex.b[j]);
					}
				}
			}

			vertices.push_back(boneIndex);
			vertices.push_back(weight[0]);
			vertices.push_back(weight[1]);
			vertices.push_back(weight[2]);
			vertices.push_back(weight[3]);
		}

		v.f = uv.x; vertices.push_back(v);
		v.f = uv.y; vertices.push_back(v);
	}
	// **�C���f�b�N�X�f�[�^�̕ϊ�**
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices == 3)
		{
			indices.emplace_back(face.mIndices[0]);
			indices.emplace_back(face.mIndices[1]);
			indices.emplace_back(face.mIndices[2]);
		}
	}

	radius = Math::Sqrt(radius);
	//�e�N�X�`���ƃ}�e���A���̓ǂݍ���
	std::unordered_map<string, Texture*> loadedTextures;
	string texFile = "MaterialTexure.png";
	// ���b�V���Ɋ֘A�t����ꂽ�}�e���A�����擾
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiString texturePath;
	//�t�@�C����FBX�����邩
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
	{
		texFile = texturePath.C_Str();

		// ���ߍ��݃e�N�X�`�����ǂ����`�F�b�N
		if (texFile[0] == '.') {
			int embeddedIndex = std::atoi(texFile.c_str() + 1);
			if (embeddedIndex < scene->mNumTextures) {
				aiTexture* embeddedTex = scene->mTextures[embeddedIndex];

				Texture* newTex = new Texture();
				if (newTex->LoadFromAssimp(embeddedTex)) {
					loadedTextures[texFile] = newTex;
				}
				else {
					newTex->Unload();
					delete newTex;
				}
			}
		}
		// �ʏ�̊O���e�N�X�`��
		else {
			if (loadedTextures.find(texFile) == loadedTextures.end())
			{
				Texture* newTex = new Texture();
				if (newTex->Load(File_P::ModelTexturePath + texFile))
				{
					loadedTextures[texFile] = newTex;
				}
				else {
					newTex->Unload();
					delete newTex;
				}
			}
		}

		if (loadedTextures.find(texFile) != loadedTextures.end())
		{
			mTextures.push_back(loadedTextures[texFile]);
		}
	}
	//�Ȃ��Ȃ�}�e���A���p�̃e�N�X�`�����[�h
	else
	{
		//�}�e���A���p�̃e�N�X�`���擾
		if (loadedTextures.find(texFile) == loadedTextures.end())
		{
			Texture* newTex = new Texture();
			if (newTex->Load(File_P::ModelTexturePath + texFile))
			{
				loadedTextures[texFile] = newTex;
			}
			else {
				newTex->Unload();
				delete newTex;
			}
		}

		if (loadedTextures.find(texFile) != loadedTextures.end())
		{
			mTextures.push_back(loadedTextures[texFile]);
		}
	}

	MaterialInfo info
	{
			Vector4(0,0,0,0),
			Vector3(0,0,0),
			Vector3(0,0,0),
			Vector3(0,0,0),
			0
	};

	aiColor4D diffuseColor;
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
	{

		info.Color = Vector4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
	}

	// �g�U�F�iDiffuse Color�j�̎擾
	aiColor3D diffuse(1.0f, 1.0f, 1.0f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

	// �����iAmbient Color�j�̎擾
	aiColor3D ambient(0.2f, 0.2f, 0.2f);
	material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

	// ���ʔ��ˁiSpecular Color�j�̎擾
	aiColor3D specular(0.5f, 0.5f, 0.5f);
	material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

	// �V�F�[�_�[�ɒl�𑗂�iglUniform3f ���g�p�j
	info.Ambient = Vector3(ambient.r, ambient.g, ambient.b);
	info.Diffuse = Vector3(diffuse.r, diffuse.g, diffuse.b);
	info.Specular = Vector3(specular.r, specular.g, specular.b);

	mMaterialInfo.push_back(info);

	float shininess = 0.0f;
	if (scene->HasMaterials())
	{

		if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess))
		{
			// �f�t�H���g�l��ݒ�
			shininess = 100.0f;
		}
		shininess = shininess / 128.0f;

		info.Shininess = shininess;
	}

	//Skin�̏ꍇ��Layout�ύX
	VertexArray::Layout layout = VertexArray::PosNormTex;
	unsigned int vertexCount = static_cast<unsigned>(vertices.size()) / MeshLayout::MESH_VERTEXCOUNT;
	if (mesh->HasBones())
	{
		layout = VertexArray::PosNormSkinTex;
		vertexCount = static_cast<unsigned>(vertices.size()) / MeshLayout::SKINMESH_VERTEXCOUNT;
	}

	mRadiusArray.push_back(radius);
	mBoxs.push_back(box);
	mOBBBoxs.push_back(obbBox);

	VertexArray* va = new VertexArray(vertices.data(), vertexCount, layout, indices.data(), static_cast<unsigned>(indices.size()));
	//���_�z��̍쐬
	mVertexArrays.push_back(va);



	//fileName����Path����������菜��
	string result = StringConverter::RemoveString(fileName, File_P::ModelPath);


	//�o�C�i���ɕϊ�
	MeshBinHeader header;
	header.layoutType = (layout == VertexArray::PosNormTex) ? 0 : 1;
	header.vertexCount = vertexCount;
	header.indexCount = static_cast<uint32_t>(indices.size());


	header.min = box.mMin;
	header.max = box.mMax;
	header.colliderRadius = radius;          // ���a�v�Z�ς݂Ɖ���
	

	result = StringConverter::RemoveExtension(result);
	string number = std::to_string(index);
	std::ofstream out(File_P::BinaryFilePath + result + number + File_P::BinaryPath, std::ios::binary);
	out.write((char*)&header, sizeof(header));
	out.write((char*)vertices.data(), sizeof(Vertex)* vertices.size());
	out.write((char*)indices.data(), sizeof(uint32_t)* indices.size());

	//�ǂݍ��ݐ���
	return true;
}

void Mesh::Unload()
{
	// �e�N�X�`���̉��
	for(int i = 0; i < mTextures.size(); i++)
	{
		if (mTextures[i] != nullptr)
		{
			mTextures[i]->Unload();
			delete mTextures[i];
			mTextures[i] = nullptr;
		}
	}
	mTextures.clear();
	for(int i = 0; i < mVertexArrays.size(); i++)
	{
		if (mVertexArrays[i] != nullptr)
		{
			delete mVertexArrays[i];
			mVertexArrays[i] = nullptr;
		}
	}
	mVertexArrays.clear();

	mBoxs.clear();
	mOBBBoxs.clear();
	mRadiusArray.clear();
	mMaterialInfo.clear();
	mShaderName.clear();
}

Texture* Mesh::GetTexture(size_t index)
{
	if (index < mTextures.size())
	{
		return mTextures[index];
	}
	else
	{
		return nullptr;
	}
}
