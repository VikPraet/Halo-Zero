#pragma once
#include <string>
class Mix_Chunk;
class SoundEffect3 final
{
public:
	explicit SoundEffect3(const std::string& path);
	~SoundEffect3();
	SoundEffect3(const SoundEffect3& other) = delete;
	SoundEffect3& operator=(const SoundEffect3& rhs) = delete;
	SoundEffect3(SoundEffect3&& other) = delete;
	SoundEffect3& operator=(SoundEffect3&& rhs) = delete;

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

