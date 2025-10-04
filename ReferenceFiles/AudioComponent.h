#pragma once
#include "Component.h"
#include "SoundEventClip.h"
#include <vector>
#include <string>

/*
* ===エンジン内部処理/Engine internal processing===
*/

//オーディオ機能をまとめたクラス
//3D、2Dのオーディオ処理を行う
class AudioComponent : public Component
{
public:
						AudioComponent(ActorObject* owner, int updateOrder = 200);
						~AudioComponent();

	void				Update(float deltaTime) override;
	void				OnUpdateWorldTransform() override;

	SoundEventClip		LoadAudio(const string& name);
	void				StopAllEvents();
private:
	vector<SoundEventClip>	mEvents2D;
	vector<SoundEventClip>	mEvents3D;
};