#pragma once
#include "utils.h"
#include "SoundEffect.h"

class Texture;
class BaseButton
{
public:
	BaseButton(const std::string& buttonText, const Point2f& centerPos);
	~BaseButton();

	virtual void Draw() const;

	virtual void OnHover(const Point2f& mousePos);

	virtual void SetSFXVolume(int volume);

protected:
	const int		m_FontSize;
	Color4f			m_TextColorActive;
	Color4f			m_TextColorInActive;
	std::string		m_pButtonText;
	Texture*		m_Texture;
	const Point2f	m_CenterPos;
	Rectf			m_DstRect;

	// sounds
	bool			m_CanPlayHoverEffect;
	SoundEffect*	m_pHoverEffect;
	SoundEffect*	m_pClickEffect;
};

