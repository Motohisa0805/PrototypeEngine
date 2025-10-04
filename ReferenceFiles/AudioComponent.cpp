#include "AudioComponent.h"
#include "Actor.h"
#include "BaseScene.h"
#include "AudioSystem.h"

AudioComponent::AudioComponent(ActorObject* owner, int updateOrder)
	:Component(owner, updateOrder)
{
}

AudioComponent::~AudioComponent()
{
	StopAllEvents();
}

void AudioComponent::Update(float deltaTime)
{
	Component::Update(deltaTime);

	// 無効な2Dイベントを削除する
	auto iter = mEvents2D.begin();
	while (iter != mEvents2D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents2D.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// 無効な3Dイベントを削除する
	iter = mEvents3D.begin();
	while (iter != mEvents3D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents3D.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void AudioComponent::OnUpdateWorldTransform()
{
	// 3Dイベントの世界が変わるアプデ
	Matrix4 world = mOwner->GetWorldTransform();
	for (auto& event : mEvents3D)
	{
		if (event.IsValid())
		{
			event.Set3DAttributes(world);
		}
	}
}

SoundEventClip AudioComponent::LoadAudio(const string& name)
{
	SoundEventClip e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
	// 2D or 3D?
	if (e.Is3D())
	{
		mEvents3D.emplace_back(e);
		// 初期の3D属性を設定する
		e.Set3DAttributes(mOwner->GetWorldTransform());
	}
	else
	{
		mEvents2D.emplace_back(e);
	}
	return e;
}

void AudioComponent::StopAllEvents()
{
	// すべての音を停止
	for (auto& e : mEvents2D)
	{
		e.Stop();
	}
	for (auto& e : mEvents3D)
	{
		e.Stop();
	}
	// イベントをクリア
	mEvents2D.clear();
	mEvents3D.clear();
}