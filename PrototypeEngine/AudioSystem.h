#pragma once
#include "SoundEventClip.h"
#include "Math.h"
#include "Typedefs.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

namespace AudioSystemLayout
{
	// 同時発音の最大数
	constexpr int MAX_CHANNELS = 512;
}

// FMODヘッダーを含めるのを避けるための前方宣言
namespace FMOD
{
	class System;
	namespace Studio
	{
		class Bank;
		class EventDescription;
		class EventInstance;
		class System;
		class Bus;
	};
};

//前方宣言
class BaseScene;

//ゲームで使用するSE、BGMをバンクとして読み込み管理するクラス
class AudioSystem
{
protected:
	friend class SoundEventClip;
	FMOD::Studio::EventInstance*										GetEventInstance(unsigned int id);
private:
	// イベントインスタンスに使用する次のIDを追跡
	static unsigned int													sNextID;

	BaseScene*															mGame;
	// Map of loaded banks
	std::unordered_map<string, FMOD::Studio::Bank*>						mBanks;
	// イベント名とイベント説明のマップ
	std::unordered_map<string, FMOD::Studio::EventDescription*>			mEvents;
	// イベントIDからイベントインスタンスへのマップ
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*>		mEventInstances;
	// Map of buses
	std::unordered_map<string, FMOD::Studio::Bus*>						mBuses;
	// FMOD studio system
	FMOD::Studio::System*												mSystem;
	// FMOD Low-level system (in case needed)
	FMOD::System*														mLowLevelSystem;
public:
																		AudioSystem(BaseScene* game);
																		~AudioSystem();

	bool																Initialize();
	void																Shutdown();

	// Load/unload banks
	void																LoadBank(const string& name);
	void																UnloadBank(const string& name);
	void																UnloadAllBanks();
	//stringで取得出来たオーディオを再生する関数
	SoundEventClip															PlayEvent(const string& name);

	void																Update(float deltaTime);

	// ポジショナルオーディオ用
	void																SetListener(const Matrix4& viewMatrix);
	// 制御バス
	float																GetBusVolume(const string& name) const;
	bool																GetBusPaused(const string& name) const;
	void																SetBusVolume(const string& name, float volume);
	void																SetBusPaused(const string& name, bool pause);
};