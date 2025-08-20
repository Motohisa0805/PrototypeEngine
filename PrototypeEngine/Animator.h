#pragma once
#include "MatrixPalette.h"
#include "Animation.h"
#include "Skeleton.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//��������A�j���[�V�������Ǘ�����N���X
//Unity�́uAnimator�v���ӎ����Đ݌v
//�A�j���[�V�����̍Đ��A�u�����h�Ȃǂ��s��
class Animator : public Component
{
private:

	MatrixPalette					mPalette;

	//�A�j���[�V������z��Ŏ����Ă�ϐ�
	vector<Animation*>				mAnimations;

	class Skeleton*					mSkeleton;

	class Animation*				mAnimation;

	class Animation*				mBlendAnimation;
	//�A�j���[�V�����̔{��
	float							mAnimPlayRate;
	//���ݍĐ����̃A�j���[�V�����̎���
	float							mAnimTime;
	//�u�����h�A�j���[�V�����̎���
	float							mBlendAnimTime;
	// ���݂̃u�����h�o�ߎ���
	float							mBlendElapsed;
	//�A�j���[�V�����̃u�����h���s�����߂̃t���O
	bool							mBlending;
public:
	Animator(class ActorObject* owner);
	~Animator();
	//�A�j���[�V�����ǂݍ��ݏ���
	bool							Load(const string& fileName,bool animLoop = 0,bool rootMotion = 0);
	// �A�j���[�V�����̍X�V����
	void							Update(float deltaTime)override;
	// �A�j���[�V�������Đ����܂��B
	// �A�j���[�V�����̒�����Ԃ��܂��B
	float							PlayAnimation(class Animation* anim);
	// �u�����h�A�j���[�V�������Đ����܂��B
	float							PlayBlendAnimation(class Animation* anim);
	//�X�P���g���ƃA�j���[�V�������g���ăX�L�j���O�s��ipalette�j�ƃ{�[���̉����X�V���s��
	void							ComputeMatrixPalette();
	//�u�����h�A�j���[�V�����̃X�L�j���O�s��ipalette�j�ƃ{�[���̉����X�V���s��
	void							BlendComputeMatrixPalette();
	//Getter
	MatrixPalette&					GetPalette() { return mPalette; }
	//�A�j���[�V�����̔z���Getter
	vector<Animation*>				GetAnimations() { return mAnimations; }
	//�X�P���g����Getter
	class Skeleton*					GetSkeleton() { return mSkeleton; }
	//�X�P���g����Setter
	void							SetSkeleton(class Skeleton* skeleton);
	//���ݍĐ����̃A�j���[�V������Getter
	class Animation*				GetAnimation() { return mAnimation; }
	//�u�����h�A�j���[�V������Getter
	class Animation*				GetBlendAnimation() { return mBlendAnimation; }
	//�A�j���[�V�����̔{��
	float							GetAnimPlayRate() { return mAnimPlayRate; }
	//�A�j���[�V�����̔{�����Z�b�g����֐�
	void							SetAnimPlayRate(float playRate) { mAnimPlayRate = playRate; }
	//���ݍĐ����̃A�j���[�V�����̎���
	float							GetAnimTime() { return mAnimTime; }
	//�u�����h�A�j���[�V�����̎���
	float							GetBlendAnimTime() { return mBlendAnimTime; }
	// ���݂̃u�����h�o�ߎ���
	float							GetBlendElapsed() { return mBlendElapsed; }
	//�u�����h�A�j���[�V�����̎��Ԃ��Z�b�g����֐�
	void							SetBlendElapsed(float elapsed) { mBlendElapsed = elapsed; }
	//���݂̃A�j���[�V�����̎��Ԃ𐳋K�������l���o�͂���֐�
	float							GetNormalizedTime();
	//Setter
	//�A�j���[�V�����̃u�����h���s�����߂̃t���O
	bool							IsBlending() { return mBlending; }
};

