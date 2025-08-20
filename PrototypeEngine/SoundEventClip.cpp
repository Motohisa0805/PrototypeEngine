#include "SoundEventClip.h"
#include "AudioSystem.h"
#include <fmod_studio.hpp>

SoundEventClip::SoundEventClip(class AudioSystem* system, unsigned int id)
	:mSystem(system)
	, mID(id)
{
}

SoundEventClip::SoundEventClip()
	:mSystem(nullptr)
	, mID(0)
{
}

bool SoundEventClip::IsValid()
{
	return (mSystem && mSystem->GetEventInstance(mID) != nullptr);
}

void SoundEventClip::ResetStart()
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->start();
	}
}

void SoundEventClip::Stop(bool allowFadeOut /* true */)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		FMOD_STUDIO_STOP_MODE mode = allowFadeOut ?
			FMOD_STUDIO_STOP_ALLOWFADEOUT :
			FMOD_STUDIO_STOP_IMMEDIATE;
		event->stop(mode);
	}
}

void SoundEventClip::SetPaused(bool pause)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPaused(pause);
	}
}

void SoundEventClip::Pause()
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPaused(true);
	}
}

void SoundEventClip::Restart()
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPaused(false);
	}
}

void SoundEventClip::SetVolume(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setVolume(value);
	}
}

void SoundEventClip::SetPitch(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPitch(value);
	}
}

void SoundEventClip::SetParameter(const string& name, float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setParameterValue(name.c_str(), value);
	}
}

bool SoundEventClip::GetPaused() const
{
	bool retVal = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPaused(&retVal);
	}
	return retVal;
}

float SoundEventClip::GetVolume() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getVolume(&retVal);
	}
	return retVal;
}

float SoundEventClip::GetPitch() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPitch(&retVal);
	}
	return retVal;
}

float SoundEventClip::GetParameter(const string& name)
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getParameterValue(name.c_str(), &retVal);
	}
	return retVal;
}

bool SoundEventClip::Is3D() const
{
	bool retVal = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		// Get the event description
		FMOD::Studio::EventDescription* ed = nullptr;
		event->getDescription(&ed);
		if (ed)
		{
			ed->is3D(&retVal);
		}
	}
	return retVal;
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

void SoundEventClip::Set3DAttributes(const Matrix4& worldTrans)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		FMOD_3D_ATTRIBUTES attr;
		// Set position, forward, up
		attr.position = VecToFMOD(worldTrans.GetTranslation());
		// In world transform, first row is forward
		attr.forward = VecToFMOD(worldTrans.GetXAxis());
		// Third row is up
		attr.up = VecToFMOD(worldTrans.GetZAxis());
		// Set velocity to zero (fix if using Doppler effect)
		attr.velocity = { 0.0f, 0.0f, 0.0f };
		event->set3DAttributes(&attr);
	}
}