#pragma once
#include "SDOpenLib.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//書籍元を改造したファイル
//SE,BGMを再生するクラス
//AudioSystem内にあるAudioBankから
//BGM、SEを取得再生する
//取得後自動で再生されるようになっている
class SoundEventClip
{
protected:
	// このコンストラクタをprotectedにし、AudioSystemを友達として設定して、
	// AudioSystemのみがこのコンストラクタにアクセスできるようにします。
	friend class		AudioSystem;
						SoundEventClip(class AudioSystem* system, unsigned int id);
private:
	class AudioSystem*	mSystem;
	unsigned int		mID;
public:
						SoundEventClip();
	// 関連するFMODイベントがまだ存在する場合、trueを返します。
	bool				IsValid();
	// イベントを最初から再起動する。
	void				ResetStart();
	// このイベントを停止。
	void				Stop(bool allowFadeOut = true);
	// Setters
	//オーディオの一時停止、再生を行う関数
	void				SetPaused(bool pause);
	//一時停止だけ出来る関数
	void				Pause();
	//停止した所から再開出来る関数
	void				Restart();
	void				SetVolume(float value);
	void				SetPitch(float value);
	void				SetParameter(const string& name, float value);
	// Getters
	bool				GetPaused() const;
	float				GetVolume() const;
	float				GetPitch() const;
	float				GetParameter(const string& name);
	// Positional
	bool				Is3D() const;
	void				Set3DAttributes(const Matrix4& worldTrans);
};