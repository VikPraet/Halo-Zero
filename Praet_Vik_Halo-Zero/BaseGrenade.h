#pragma once
#include "Level.h"
#include "SoundEffect.h"
class Avatar;
class BaseEnemy;
class Texture;
class BaseGrenade
{
public:
	BaseGrenade(const Point2f& starPos, bool isLookingLeft, float scale);
	~BaseGrenade();

	virtual void Update(float elapsedSec, Avatar*& avatar, std::vector<BaseEnemy*>& enemies, Level& level) = 0;
	virtual void Draw() const = 0;

	virtual void SetSoundVolume(int volume) = 0;

	virtual void CleanUp() = 0;

protected:
	Point2f m_StartPos;
	bool	m_IsLookingLeft;
	float	m_Scale;
};