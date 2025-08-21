#pragma once
#include "GameWinMain.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�V�[���̊��N���X
//�I�u�W�F�N�g�̍X�V�Ȃǂ��܂Ƃ߂čs�������ɂȂ�܂��B
// �V�[�����ƂɌp�����Ďg�p���܂��B
class BaseScene
{
public:
	enum MouseMode
	{
		Relative, // ���΃��[�h
		Absolute  // ��΃��[�h
	};
protected:

	AudioSystem*									mAudioSystem;

	PhysWorld*										mPhysWorld;
	// Track if we're updating actors right now
	bool											mUpdatingActors;

	// All the actors in the game
	vector<class ActorObject*>						mActors;
	vector<class Canvas*>							mCanvasStack;
	vector<class Image*>							mImageStack;
	vector<class Image*>							mDebugImageStack;
	// Map for fonts
	std::unordered_map<string, class Font*>			mFonts;
	// Map of loaded skeletons
	std::unordered_map<string, class Skeleton*>		mSkeletons;
	// Any pending actors
	vector<class ActorObject*>						mPendingActors;


	class ActorObject*								mPlayer;

	class DirectionalLightActor*					mDirectionalLightActor;

	class BaseCamera*								mMainCamera;

	// 50Hz�AUnity�Ɠ���
	const float										mFixed_Delta_Time = 0.02f;

	float											mFixedTimeAccumulator;

	//***�f�o�b�O�@�\***
	//�t���[�����[�g�\���e�L�X�g
	class Text*										mFrameRateText;
public:
	//�R���X�g���N�^
													BaseScene();
	//������
	virtual bool									Initialize();
	//���͍X�V
	virtual bool									InputUpdate(const InputState& state);
	//�Œ�X�V
	virtual bool									FixedUpdate();
	//�X�V
	virtual bool									Update();
	//���
	void											UnloadData();

	void											SetMouseMode(MouseMode mode);

	void											LoadSkyBoxTexture(string file);

	vector<class ActorObject*>&						GetActors() { return mActors; }
	//�I�u�W�F�N�g�ǉ�
	void											AddActor(class ActorObject* actor);
	//�I�u�W�F�N�g�폜
	void											RemoveActor(class ActorObject* actor);

	template<typename T>
	vector<ActorObject*>							SelectAllActorComponent();

	//Font��Getter
	class Font*										GetFont(const string& fileName);
	//�X�P���g����Getter
	class Skeleton*									GetSkeleton(const string& fileName);
	//AudioSystem��Getter
	class AudioSystem*								GetAudioSystem() { return mAudioSystem; }
	//PhysWorld��Getter
	class PhysWorld*								GetPhysWorld() { return mPhysWorld; }

	// Manage UI stack
	const vector<class Canvas*>&					GetUIStack() { return mCanvasStack; }
	//UIScreen�̐ݒ�
	void											PushUI(class Canvas* screen);
	//Image�z���Getter
	const vector<class Image*>&						GetImageStack() { return mImageStack; }
	const vector<class Image*>&						GetDebugImageStack() { return mDebugImageStack; }
	//Image�̒ǉ�
	void											PushImage(class Image* screen);
	void											RemoveImage(class Image* screen);
	//DebugImage�̒ǉ�
	void											PushDebugImage(class Image* screen);
	void											RemoveDebugImage(class Image* screen);
	// Game-specific
	//Player�I�u�W�F�N�g��Getter
	class ActorObject*								GetPlayer() { return mPlayer; }
	//�����̐ݒ�
	class DirectionalLightActor*					GetDirectionalLightActor() { return mDirectionalLightActor; }
	//Camera��Getter
	class BaseCamera*								GetCamera() { return mMainCamera; }
	//MainCamera�̐ݒ�
	void											SetMainCamera(class BaseCamera* camera) { mMainCamera = camera; }
};

template<typename T>
inline vector<ActorObject*> BaseScene::SelectAllActorComponent()
{
	std::vector<ActorObject*> result;

	for (auto* actor : mActors)
	{
		for (auto* component : actor->GetComponents())
		{
			if (dynamic_cast<T*>(component)) // T�^��Component�����邩
			{
				result.push_back(actor);
				break; // ��ł�������΂��̃I�u�W�F�N�g�͑ΏۂɂȂ�
			}
		}
	}

	return result;
}

#define Release_Function  0
#define Debug_Function  1