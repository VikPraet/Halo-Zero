#pragma once
#include "SoundEffect.h"

class Avatar;
class BaseObject
{
public:
	BaseObject(const Point2f& position, float scale);
	~BaseObject();

	virtual void Draw() const = 0;
	virtual void Update(Avatar*& avatar) = 0;

	virtual void CleanUp() = 0;

	virtual void SetSFXVolume(int volume) = 0;

protected:
	Point2f m_Position;
	float	m_Scale;
	const Rectf m_levelRect;
};

