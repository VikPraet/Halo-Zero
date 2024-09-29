#pragma once
#include <string>
class Mix_Chunk;
class SoundEffect2 final
{
public:
	explicit SoundEffect2(const std::string& path);
	~SoundEffect2();
	SoundEffect2(const SoundEffect2& other) = delete;
	SoundEffect2& operator=(const SoundEffect2& rhs) = delete;
	SoundEffect2(SoundEffect2&& other) = delete;
	SoundEffect2& operator=(SoundEffect2&& rhs) = delete;

	bool IsLoaded() const;
	bool Play(const int loops) const;
	void SetVolume(const int value);
	int GetVolume() const;
	static void StopAll();
	static void PauseAll();
	static void ResumeAll();

private:
	Mix_Chunk* m_pMixChunk;
};


