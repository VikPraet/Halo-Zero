#include "base.h"
#include <iostream>
#include "SoundEffect2.h"

SoundEffect2::SoundEffect2(const std::string& path)
	:m_pMixChunk{ Mix_LoadWAV(path.c_str()) }
{
	if (m_pMixChunk == nullptr)
	{
		const std::string errorMsg = "SoundEffect: Failed to load " + path + ",\nSDL_mixer Error: " + Mix_GetError();
		std::cerr << errorMsg;
	}
}
SoundEffect2::~SoundEffect2()
{
	Mix_FreeChunk(m_pMixChunk);
	m_pMixChunk = nullptr;
}

bool SoundEffect2::IsLoaded() const
{
	return m_pMixChunk != nullptr;
}

bool SoundEffect2::Play(const int loops) const
{
	// Don't save the channel as a data member, 
	// because when it stops playing the channel becomes free
	// and available for usage by other effects
	if (m_pMixChunk != nullptr)
	{
		const int channel{ Mix_PlayChannel(62, m_pMixChunk, loops) };
		return channel == 62 ? false : true;
		std::cout << channel << "\n";
	}
	else
	{
		return false;
	}
}

void SoundEffect2::SetVolume(const int value)
{
	if (m_pMixChunk != nullptr)
	{
		Mix_VolumeChunk(m_pMixChunk, value);
	}
}

int SoundEffect2::GetVolume() const
{
	if (m_pMixChunk != nullptr)
	{
		return Mix_VolumeChunk(m_pMixChunk, -1);
	}
	else
	{
		return -1;
	}
}

void SoundEffect2::StopAll()
{
	Mix_HaltChannel(62);
}

void SoundEffect2::PauseAll()
{
	Mix_Pause(62);
}
void SoundEffect2::ResumeAll()
{
	Mix_Resume(62);
}




