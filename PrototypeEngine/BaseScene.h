#pragma once
#include "GameWinMain.h"

/*
* ===�G���W����������/Engine internal processing===
*/

//�O���錾
class ActorObject;
class Canvas;
class Image;
class Font;
class Skeleton;
class DirectionalLightActor;
class BaseCamera;
class DirectionalLightComponent; //���ǉ�
class FreeCamera;               // ���ǉ�
class Text;
//�V�[���̊��N���X
//�I�u�W�F�N�g�̍X�V�Ȃǂ��܂Ƃ߂čs�������ɂȂ�܂��B
// �V�[�����ƂɌp�����Ďg�p���܂��B
class BaseScene
{
protected:
	//ActorObject���R���X�g���N�^�ŌĂяo�����߂̊֐�
	friend class ActorObject;
	// Track if we're updating actors right now
	bool											mUpdatingActors;

	AudioSystem*									mAudioSystem;

	PhysWorld*										mPhysWorld;

	// Any pending actors
	vector<ActorObject*>							mPendingActors;

	// All the actors in the game
	vector<ActorObject*>							mActors;
	
	
	vector<Canvas*>									mCanvasStack;
	
	vector<Image*>									mImageStack;
	
	vector<Image*>									mDebugImageStack;

	// Map for fonts
	std::unordered_map<string, Font*>				mFonts;
	// Map of loaded skeletons
	std::unordered_map<string, Skeleton*>			mSkeletons;

	std::unordered_map<string, BaseCamera*>			mCameras;
	
	//Actor�Ɋ��蓖�Ă郆�j�[�N��ID/�J�E���g
	int												mNextActorID;
	

	ActorObject*									mPlayer;

	// 50Hz�AUnity�Ɠ���
	const float										mFixed_Delta_Time = 0.02f;

	float											mFixedTimeAccumulator;
	//***�f�o�b�O�@�\***
	//�t���[�����[�g�\���e�L�X�g
	Text*											mFrameRateText;


	//�V�[���̖��O
	string											mName;
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
	virtual bool									EditorUpdate();
	//���
	void											UnloadData();

	void											LoadSkyBoxTexture(string file);

	const vector<ActorObject*>&						GetActors()const { return mActors; }
	//�I�u�W�F�N�g�ǉ�
	void											AddActor(ActorObject* actor);
	//�I�u�W�F�N�g�폜
	void											RemoveActor(ActorObject* actor);
	void											DeleteActor(ActorObject* actor);
	//�X�V�҂��̃I�u�W�F�N�g�����C�����X�g�Ɉړ�����֐�
	void											ProcessPendingActors();

	template<typename T>
	vector<ActorObject*>							SelectAllActorComponent();

	//Font��Getter
	Font*											GetFont(const string& fileName);
	//�X�P���g����Getter
	Skeleton*										GetSkeleton(const string& fileName);
	//AudioSystem��Getter
	AudioSystem*									GetAudioSystem() { return mAudioSystem; }
	//PhysWorld��Getter
	PhysWorld*										GetPhysWorld() { return mPhysWorld; }

	// Manage UI stack
	const vector<Canvas*>&							GetUIStack() { return mCanvasStack; }
	//UIScreen�̐ݒ�
	void											PushUI(Canvas* screen);
	//Image�z���Getter
	const vector<Image*>&							GetImageStack() { return mImageStack; }
	const vector<Image*>&							GetDebugImageStack() { return mDebugImageStack; }
	//Image�̒ǉ�
	void											PushImage(Image* screen);
	void											RemoveImage(Image* screen);
	//DebugImage�̒ǉ�
	void											PushDebugImage(Image* screen);
	void											RemoveDebugImage(Image* screen);
	// Game-specific
	//Player�I�u�W�F�N�g��Getter
	ActorObject*									GetPlayer() { return mPlayer; }

	//Camera�̒ǉ�
	void											AddCamera(BaseCamera* camera);
	//Camera�̍폜
	void											RemoveCamera(BaseCamera* camera);
	//Camera�̎擾
	BaseCamera*										GetCamera(const string& name = "Camera0");

	std::unordered_map<string, BaseCamera*> 		GetCameras() { return mCameras; }

	//�V�[�����̃A�N�e�B�u�ȕ��������C�g�R���|�[�l���g���擾
	DirectionalLightComponent*						GetActiveDirectionalLightComponent();

	//�V�[�����̃��C���J�����R���|�[�l���g���擾
	BaseScene*										GetMainCameraComponent();

	//�V�[������Getter
	string											GetName() { return mName; }
	//�V�[������Setter
	virtual void									SetName(const string& name) { mName = name; }

	bool										    mIsComputeWorldTransform;
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

// ActorObject���X�g��ێ�����EditorScene�N���X������
class EditorScene : public BaseScene
{
public:

	void EditorInitilaize();
	// ... BaseScene �̉��z�֐������� 
	void SetName(const string& name)override;
};

#define Release_Function  0
#define Debug_Function  1