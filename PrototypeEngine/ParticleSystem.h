#pragma once
#include "Component.h"
#include "Shader.h"
#include "Actor.h"
#include "Texture.h"
#include "Random.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�p�[�e�B�N���̍\����
struct ParticleStruct
{
	Vector3 mPosition;
	Vector3 mVelocity;
	float	mMaxLifetime;
	float	mLifetime;
	float	mSize;
	bool	mActive;
	Vector4 mColor;
};
// �p�[�e�B�N���V�X�e���N���X
class ParticleSystem : public Component
{
protected:
	// �p�[�e�B�N���̃C���X�^���X
	vector<ParticleStruct*> mParticle;
	// �p�[�e�B�N���̃e�N�X�`��
	class Texture*			mParticleTexture;
	//
	bool					mIsAlphaFade;
	// �p�[�e�B�N���̃��[�v�t���O
	bool					mIsLoop; 
	// �p�[�e�B�N���̉����t���O
	bool					mVisible;
	// �p�[�e�B�N���̔j���t���O
	bool					mIsDestroyed;
	// �ő�p�[�e�B�N����
	int						mMaxParticleCount; 
	// �p�[�e�B�N���̃��C�t�^�C��
	float					mParticleLifeTime;
	// �p�[�e�B�N���̑S�̃��C�t�^�C��
	float					mParticleAllLifeTime;
	// �p�[�e�B�N���̔��ˑ��x
	Vector3					mParticleEmitSpeed;
	// �p�[�e�B�N���̃T�C�Y
	// �p�[�e�B�N���̍ő�T�C�Y
	float					mParticleMaxSize;
	// �p�[�e�B�N���̍ŏ��T�C�Y
	float					mParticleMinSize;
	// �p�[�e�B�N���̃f�t�H���g�F
	Vector4					mDefaultColor; // �f�t�H���g�͔��F
	// �p�[�e�B�N���̔��ˊԊu
	float					mEmitInterval = 0.02f; // 50��/�b
	// �p�[�e�B�N���̔��˃^�C�}�[
	float					mEmitTimer = 0.0f;
public:
	ParticleSystem(class ActorObject* owner);
	~ParticleSystem();
	// �p�[�e�B�N���V�X�e���̍X�V
	void						Update(float deltaTime)override;
	// �p�[�e�B�N���̕`��
	virtual void				Draw(class Shader* shader);
	// �e�N�X�`���̓ǂݍ���
	void						LoadTexture(string name);
	//���x��^����
	Vector3						AddVelocity();
	// �p�[�e�B�N���̔���
	void						Emit();
	// �p�[�e�B�N�����擾
	vector<ParticleStruct*>		GetParticle() const { return mParticle; }
	//�p�[�e�B�N���̉����̎擾
	bool						IsVisible() const { return mVisible; }
	// �����̐ݒ�
	void						SetVisible(bool visible) { mVisible = visible; }
	// �A���t�@�t�F�[�h�̎擾
	bool 						IsAlphaFade() const { return mIsAlphaFade; }
	// �A���t�@�t�F�[�h�̐ݒ�
	void 						SetAlphaFade(bool alphaFade) { mIsAlphaFade = alphaFade; }
	// ���[�v�t���O�̎擾
	bool 						IsLoop() const { return mIsLoop; }
	// ���[�v�t���O�̐ݒ�
	void						SetLoop(bool loop) { mIsLoop = loop; }
	// �j���t���O�̎擾
	bool 						IsDestroyed() const { return mIsDestroyed; }
	// �j���t���O�̐ݒ�
	void						SetDestroyed(bool destroyed) { mIsDestroyed = destroyed; }
	// �ő�p�[�e�B�N�����̎擾
	void						SetParticleSpeed(Vector3 speed);
	// �p�[�e�B�N���̑��x�ݒ�
	void						SetColor(Vector4 color);
	// �p�[�e�B�N���̐F�ݒ�
	void						SetParticleLifeTime(float lifeTime) { mParticleLifeTime = lifeTime; }
	
	float						GetParticleMaxSize() const { return mParticleMaxSize; }

	float						GetParticleMinSize() const { return mParticleMinSize; }

	void						SetParticleSize(float min,float max) 
	{
		mParticleMinSize = min;
		mParticleMaxSize = max; 
	}

	ParticleStruct*				GetInactiveParticleOrCreateNew();

	void SetEmitInterval(float interval);
};

