#include "AudioSystem.h"
#include <SDL3/SDL_log.h>
#include <fmod_studio.hpp>
#include <fmod_errors.h>

unsigned int AudioSystem::sNextID = 0;

AudioSystem::AudioSystem(BaseScene* game)
	:mGame(game)
	, mSystem(nullptr)
	, mLowLevelSystem(nullptr)
{
}

AudioSystem::~AudioSystem()
{
}

bool AudioSystem::Initialize()
{
	// デバッグログを初期化する
	FMOD::Debug_Initialize(
		FMOD_DEBUG_LEVEL_ERROR, // Log only errors
		FMOD_DEBUG_MODE_TTY // Output to stdout
	);

	// FMODスタジオシステムオブジェクトを作成
	FMOD_RESULT result;
	result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to create FMOD system: %s", FMOD_ErrorString(result));
		return false;
	}

	// FMODスタジオシステムを初期化
	result = mSystem->initialize(
		AudioSystemLayout::MAX_CHANNELS, // 同時発音の最大数
		FMOD_STUDIO_INIT_NORMAL, // デフォルト設定を使用する
		FMOD_INIT_NORMAL, // デフォルト設定を使用する
		nullptr // 通常は無効
	);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to initialize FMOD system: %s", FMOD_ErrorString(result));
		return false;
	}

	// 低レベルシステムポインタを保存
	mSystem->getLowLevelSystem(&mLowLevelSystem);

	// マスターバンクをロードします（最初に文字列を）
	LoadBank(AudioBankProperty::GetMasterBankString());
	LoadBank(AudioBankProperty::GetMasterBank());

	return true;
}

void AudioSystem::Shutdown()
{
	// すべてのイベントインスタンスを明示的に解放
	for (auto& ei : mEventInstances)
	{
		if (ei.second)
		{
			ei.second->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			ei.second->release();
		}
	}
	mEventInstances.clear();
	// すべてのバンクをアンロードしてください
	UnloadAllBanks();
	// FMODシステムをシャットダウン
	if (mSystem)
	{
		mSystem->release();
	}
}

void AudioSystem::LoadBank(const string& name)
{
	string path = AudioFile::AudioFilePath + name;
	// 二重ロードを防ぐ
	if (mBanks.find(path) != mBanks.end())
	{
		return;
	}

	// バンクを読み込もうとしてください
	FMOD::Studio::Bank* bank = nullptr;
	FMOD_RESULT result = mSystem->loadBankFile(
		path.c_str(), // バンクのファイル名
		FMOD_STUDIO_LOAD_BANK_NORMAL, // 通常の読み込み
		&bank // バンクへのポインタを保存する
	);

	const int maxPathLength = AudioSystemLayout::MAX_CHANNELS;
	if (result == FMOD_OK)
	{
		// mapにバンクを追加する
		mBanks.emplace(path, bank);
		// すべての非ストリーミングサンプルデータをロードする
		bank->loadSampleData();
		// このバンクのイベント数を取得する
		int numEvents = 0;
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// このバンクのイベントの説明のリストを取得します
			vector<FMOD::Studio::EventDescription*> events(numEvents);
			bank->getEventList(events.data(), numEvents, &numEvents);
			char eventName[maxPathLength];
			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// このイベントのパスを取得します
				// （例：event:/Explosion2D）
				e->getPath(eventName, maxPathLength, nullptr);
				//イベントをmapに追加
				mEvents.emplace(eventName, e);
			}
		}
		// このバンクにあるバスの数を取得してください
		int numBuses = 0;
		bank->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			// このバンクのバスのリストを取得する
			vector<FMOD::Studio::Bus*> buses(numBuses);
			bank->getBusList(buses.data(), numBuses, &numBuses);
			char busName[AudioSystemLayout::MAX_CHANNELS];
			for (int i = 0; i < numBuses; i++)
			{
				FMOD::Studio::Bus* bus = buses[i];
				// このバスのパスを取得してください（bus:/SFXのように）
				bus->getPath(busName, AudioSystemLayout::MAX_CHANNELS, nullptr);
				//バスをmapに追加
				mBuses.emplace(busName, bus);
			}
		}
	}
}

void AudioSystem::UnloadBank(const string& name)
{
	string path = AudioFile::AudioFilePath + name;
	// Ignore if not loaded
	auto iter = mBanks.find(path);
	if (iter == mBanks.end())
	{
		return;
	}

	// First we need to remove all events from this bank
	FMOD::Studio::Bank* bank = iter->second;
	int numEvents = 0;
	bank->getEventCount(&numEvents);
	if (numEvents > 0)
	{
		// Get event descriptions for this bank
		vector<FMOD::Studio::EventDescription*> events(numEvents);
		// Get list of events
		bank->getEventList(events.data(), numEvents, &numEvents);
		char eventName[AudioSystemLayout::MAX_CHANNELS];
		for (int i = 0; i < numEvents; i++)
		{
			FMOD::Studio::EventDescription* e = events[i];
			// Get the path of this event
			e->getPath(eventName, AudioSystemLayout::MAX_CHANNELS, nullptr);
			// Remove this event
			auto eventi = mEvents.find(eventName);
			if (eventi != mEvents.end())
			{
				mEvents.erase(eventi);
			}
		}
	}
	// Get the number of buses in this bank
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0)
	{
		// Get list of buses in this bank
		vector<FMOD::Studio::Bus*> buses(numBuses);
		bank->getBusList(buses.data(), numBuses, &numBuses);
		char busName[AudioSystemLayout::MAX_CHANNELS];
		for (int i = 0; i < numBuses; i++)
		{
			FMOD::Studio::Bus* bus = buses[i];
			// Get the path of this bus (like bus:/SFX)
			bus->getPath(busName, AudioSystemLayout::MAX_CHANNELS, nullptr);
			// Remove this bus
			auto busi = mBuses.find(busName);
			if (busi != mBuses.end())
			{
				mBuses.erase(busi);
			}
		}
	}

	// Unload sample data and bank
	bank->unloadSampleData();
	bank->unload();
	// Remove from banks map
	mBanks.erase(iter);
}

void AudioSystem::UnloadAllBanks()
{
	for (auto& iter : mBanks)
	{
		// Unload the sample data, then the bank itself
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	mBanks.clear();
	// No banks means no events
	mEvents.clear();
}

SoundEventClip AudioSystem::PlayEvent(const string& name)
{
	unsigned int retID = 0;
	auto iter = mEvents.find(name);
	if (iter != mEvents.end())
	{
		// Create instance of event
		FMOD::Studio::EventInstance* event = nullptr;
		iter->second->createInstance(&event);
		if (event)
		{
			// BGM、SEを再生
			event->start();
			// Get the next id, and add to map
			sNextID++;
			retID = sNextID;
			mEventInstances.emplace(retID, event);
		}
	}
	return SoundEventClip(this, retID);
}

void AudioSystem::Update(float deltaTime)
{
	// Find any stopped event instances
	vector<unsigned int> done;
	for (auto& iter : mEventInstances)
	{
		FMOD::Studio::EventInstance* e = iter.second;
		// Get the state of this event
		FMOD_STUDIO_PLAYBACK_STATE state;
		e->getPlaybackState(&state);
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			// Release the event and add id to done
			e->release();
			done.emplace_back(iter.first);
		}
	}

	// Remove done event instances from map
	for (auto id : done)
	{
		mEventInstances.erase(id);
	}

	// Update FMOD
	mSystem->update();
}

namespace
{
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		// Convert from our coordinates (+x forward, +y right, +z up)
		// to FMOD (+z forward, +x right, +y up)
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

void AudioSystem::SetListener(const Matrix4& viewMatrix)
{
	// Invert the view matrix to get the correct vectors
	Matrix4 invView = viewMatrix;
	invView.Invert();
	FMOD_3D_ATTRIBUTES listener;
	// Set position, forward, up
	listener.position = VecToFMOD(invView.GetTranslation());
	// In the inverted view, third row is forward
	listener.forward = VecToFMOD(invView.GetZAxis());
	// In the inverted view, second row is up
	listener.up = VecToFMOD(invView.GetYAxis());
	// Set velocity to zero (fix if using Doppler effect)
	listener.velocity = { 0.0f, 0.0f, 0.0f };
	// Send to FMOD
	mSystem->setListenerAttributes(0, &listener);
}

float AudioSystem::GetBusVolume(const string& name) const
{
	float retVal = 0.0f;
	const auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getVolume(&retVal);
	}
	return retVal;
}

bool AudioSystem::GetBusPaused(const string& name) const
{
	bool retVal = false;
	const auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getPaused(&retVal);
	}
	return retVal;
}

void AudioSystem::SetBusVolume(const string& name, float volume)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setVolume(volume);
	}
}

void AudioSystem::SetBusPaused(const string& name, bool pause)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setPaused(pause);
	}
}

FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id)
{
	FMOD::Studio::EventInstance* event = nullptr;
	auto iter = mEventInstances.find(id);
	if (iter != mEventInstances.end())
	{
		event = iter->second;
	}
	return event;
}
