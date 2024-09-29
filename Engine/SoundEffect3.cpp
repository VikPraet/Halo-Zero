#include "base.h"
#include <iostream>
#include "SoundEffect3.h"

SoundEffect3::SoundEffect3(const std::string& path)
	:m_pMixChunk{ Mix_LoadWAV(path.c_str()) }
{
	if (m_pMixChunk == nullptr)
	{
		const std::string errorMsg = "SoundEffect: Failed to load " + path + ",\nSDL_mixer Error: " + Mix_GetError();
		std::cerr << errorMsg;
	}
}
SoundEffect3::~SoundEffect3()
{
	Mix_FreeChunk(m_pMixChunk);
	m_pMixChunk = nullptr;
}

bool SoundEffect3::IsLoaded() const
{
	return m_pMixChunk != nullptr;
}

bool SoundEffect3::Play(const int loops) const
{
	// Don't save the channel as a data member, 
	// because when it stops playing the channel becomes free
	// and available for usage by other effects
	if (m_pMixChunk != nullptr)
	{
		const int channel{ Mix_PlayChannel(63, m_pMixChunk, loops) };
		return channel == 63 ? false : true;
		std::cout << channel << "\n";
	}
	else
	{
		return false;
	}
}

void SoundEffect3::SetVolume(const int value)
{
	if (m_pMixChunk != nullptr)
	{
		Mix_VolumeChunk(m_pMixChunk, value);
	}
}

int SoundEffect3::GetVolume() const
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

void SoundEffect3::StopAll()
{
	Mix_HaltChannel(63);
}

void SoundEffect3::PauseAll()
{
	Mix_Pause(63);
}
void SoundEffect3::ResumeAll()
{
	Mix_Resume(63);
}




