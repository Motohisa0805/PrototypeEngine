#pragma once
#include "Component.h"
#include "SoundEventClip.h"
#include "Typedefs.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//オーディオ機能をまとめたクラス
//3D、2Dのオーディオ処理を行う
class AudioComponent : public Component
{
private:
	vector<SoundEventClip>	mEvents2D;
	vector<SoundEventClip>	mEvents3D;
public:
						AudioComponent(ActorObject* owner, int updateOrder = 200);
						~AudioComponent();

	void				Update(float deltaTime) override;
	void				OnUpdateWorldTransform() override;

	SoundEventClip		LoadAudio(const string& name);
	void				StopAllEvents();

	void				Serialize(json& j) const override;
	void				Deserialize(const json& j)override;

	void				DrawCustomGUI(const std::vector<PropertyInfo>& properties)override;

	Component*			Clone(ActorObject* newOwner) const override;
};