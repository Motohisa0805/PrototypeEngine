#pragma once
#include "Texture.h"
#include "Renderer.h"
//�O���錾
class Renderer;
//�G���W���Ŏg�p����GUI�摜��ǂݍ��݁A�ۑ��A�������N���X
class EditorTextureManager
{
private:
	EditorTextureManager();

	Texture* mPlayButtonTexture;
	Texture* mPauseButtonTexture;
	Texture* mStopButtonTexture;
	Texture* mFrameByFrameButtonTexture;
public:
	static EditorTextureManager& GetInstance()
	{
		//�ÓI�ϐ��Ƃ��ăC���X�^���X���`
		static EditorTextureManager instance;
		return instance;
	}
	//�R�s�[�R���X�g���N�^�Ƒ�����Z�q���폜
	EditorTextureManager(const EditorTextureManager&) = delete;
	EditorTextureManager& operator = (const EditorTextureManager&) = delete;
	void AllLoad();
	void AllRelease();

	Texture* GetPlayButtonTexture()const { return mPlayButtonTexture; }
	Texture* GetPauseButtonTexture()const { return mPauseButtonTexture; }
	Texture* GetStopButtonTexture()const { return mStopButtonTexture; }
	Texture* GetFrameByFrameButtonTexture()const { return mFrameByFrameButtonTexture; }
};

