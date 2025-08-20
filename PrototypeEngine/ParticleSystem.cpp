#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ActorObject* owner)
	: Component(owner)
	, mVisible(true)
	, mIsAlphaFade(false)
	, mIsLoop(false)
	, mIsDestroyed(false)
	, mParticleLifeTime(1.0f) // �f�t�H���g�̃p�[�e�B�N�����C�t�^�C��
	, mParticleAllLifeTime(1.0f) // �f�t�H���g�̍ő�p�[�e�B�N�����C�t�^�C��
	, mMaxParticleCount(50) // �f�t�H���g�̍ő�p�[�e�B�N����
	, mParticleEmitSpeed(Vector3(1.0f, 2.0f, 1.0f)) // �f�t�H���g�̃p�[�e�B�N�����ˑ��x
	, mParticleMaxSize(1.0f) // �f�t�H���g�̃p�[�e�B�N���T�C�Y
	, mParticleMinSize(0.1f) // �f�t�H���g�̍ŏ��p�[�e�B�N���T�C�Y
	, mEmitInterval(0.02f) // 50��/�b
	, mEmitTimer(0.0f)
	, mDefaultColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)) // �f�t�H���g�̐F�i���j
	, mParticleTexture(nullptr) // ���������̓e�N�X�`���Ȃ�
{
	GameWinMain::GetRenderer()->AddParticleComp(this);

	mParticleAllLifeTime = mParticleLifeTime;

	//�������ɏ����摜��ǂݍ���
	mParticleTexture = GameWinMain::GetRenderer()->GetTexture("Assets/Particle/Default.png");
}

ParticleSystem::~ParticleSystem()
{
	GameWinMain::GetRenderer()->RemoveParticleComp(this);

	for (auto& p : mParticle)
		delete p;
	mParticle.clear();
}

void ParticleSystem::LoadTexture(string name)
{
	string path = File_P::AssetPath + "Particle/" + name;
	mParticleTexture = GameWinMain::GetRenderer()->GetTexture(path);
	if (!mParticleTexture)
	{
		SDL_Log("Failed to load particle texture: %s", path.c_str());
		return;
	}
}

Vector3 ParticleSystem::AddVelocity()
{
	Vector3 velocity = Vector3(1.0f,1.0f,1.0f);

	// Emit�֐��̒�
	velocity = Random::GetVector(
		Vector3(-0.2f, 1.5f, -0.2f), // �ŏ��l�FY�ɋ����㏸
		Vector3(0.2f, 2.5f, 0.2f)    // �ő�l�F�h�炬���o��
	) * Random::GetFloatRange(0.8f, 1.2f); // �΂��

	return velocity;
}

void ParticleSystem::Emit()
{
	ParticleStruct* p = GetInactiveParticleOrCreateNew(); // �ė��p or �V�K�쐬
	if (!p) return; // �ő吔�Ȃ�Emit���Ȃ�
	p->mActive = true;
	p->mLifetime = p->mMaxLifetime = Random::GetFloatRange(0.5f, 1.0f);
	p->mPosition = mOwner->GetLocalPosition(); // �� ���񏉊��ʒu����
	p->mVelocity = AddVelocity();
	p->mSize = Random::GetFloatRange(mParticleMinSize, mParticleMaxSize);
	p->mColor = mDefaultColor;
}

void ParticleSystem::SetParticleSpeed(Vector3 speed)
{
	mParticleEmitSpeed = speed;
}

void ParticleSystem::SetColor(Vector4 color)
{
	mDefaultColor = color; // �V�K�p�[�e�B�N���ɔ��f�����悤��
	for (auto& p : mParticle)
	{
		if (p->mActive)
			p->mColor = color;
	}
}

ParticleStruct* ParticleSystem::GetInactiveParticleOrCreateNew()
{
	if (mParticle.size() >= mMaxParticleCount)
		return nullptr;

	for (auto& p : mParticle)
	{
		if (!p->mActive)
		{
			return p; // ��A�N�e�B�u�ȃp�[�e�B�N�����ė��p
		}
	}
	// ���ׂăA�N�e�B�u�Ȃ�V�K�쐬���Ēǉ�
	ParticleStruct* newParticle = new ParticleStruct();
	mParticle.push_back(newParticle);
	return newParticle;
}

void ParticleSystem::SetEmitInterval(float interval)
{
	mEmitInterval = interval;
	mEmitTimer = 0.0f; // �^�C�}�[�����Z�b�g
}

void ParticleSystem::Update(float deltaTime)
{
	// �S�̃��C�t�^�C����0�ȉ��Ȃ�X�V���Ȃ�
	if (mParticleAllLifeTime <= 0.0f) { return; } 
	mParticleAllLifeTime -= deltaTime;
	mEmitTimer += deltaTime;

	// ���Ԋu��1������ Emit
	while (mEmitTimer >= mEmitInterval)
	{
		Emit();
		mEmitTimer -= mEmitInterval;
	}

	// �e�p�[�e�B�N���X�V
	for (auto& p : mParticle)
	{
		if (!p->mActive) continue;

		p->mLifetime -= deltaTime;
		if (p->mLifetime <= 0.0f)
		{
			p->mActive = false;
			continue;
		}

		// �ړ�
		p->mPosition += p->mVelocity * deltaTime;
	}
	// �p�[�e�B�N���̑S�̃��C�t�^�C����0�ȉ��Ȃ�A���[�v���Ȃ��ꍇ�͔j��
	if( mParticleAllLifeTime <= 0.0f)
	{
		if(!mIsLoop && mIsDestroyed)
		{
			mOwner->SetState(ActorObject::EDead);
		}
		else
		{
			mParticleAllLifeTime = mParticleLifeTime;
		}
	}
}

void ParticleSystem::Draw(Shader* shader)
{
	// �J�����̃r���[�s��̋t�s����擾�i���[���h�ϊ��s��j
	Matrix4 viewMatrix = GameWinMain::GetRenderer()->GetView();
	viewMatrix.Invert(); // �J�����̃��[���h��ԏ��

	// �J�����̌������擾
	Vector3 camRight = viewMatrix.GetXAxis(); // X�����i�E�j
	Vector3 camUp = viewMatrix.GetYAxis(); // Y�����i��j
	Vector3 camForward = viewMatrix.GetZAxis(); // Z�����i�O�j

	for(int i = 0; i < mParticle.size(); ++i)
	{
		if (mParticle[i]->mActive)
		{
			ParticleStruct* p = mParticle[i];

			// ���f���s��
			// === �r���{�[�h�p���[���h�s����쐬 ===
			Matrix4 world = Matrix4::Billboard(
				p->mPosition, 
				p->mSize, 
				camRight, 
				camUp, 
				camForward
			);
			// �����Ń��[���h�s���ݒ�
			shader->SetMatrixUniform("uWorldTransform", world);
			shader->SetVector4Uniform("uTexUV", Vector4(0.0f, 0.0f, 1.0f, 1.0f));
			// �����Ńp�[�e�B�N���̐F��ݒ�
			// mColor�͌��̐F�ŁA�A���t�@�t�F�[�h���L���ȏꍇ�͓����x�𒲐�
			Vector4 drawColor = p->mColor;
			if (mIsAlphaFade)
			{
				float alpha = p->mLifetime / p->mMaxLifetime;
				drawColor.w *= alpha; // ���� mColor.w �ɉe�����Ȃ�
			}
			shader->SetVector4Uniform("uColor", drawColor);

			//�摜���e�N�X�`���Ƃ��Ďg�p����ꍇ�́A�����Ńo�C���h���܂�
			if (!mParticleTexture) return;
			mParticleTexture->SetActive();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}
	}
}
